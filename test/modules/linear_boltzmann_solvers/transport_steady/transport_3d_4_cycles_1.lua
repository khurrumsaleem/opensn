-- 3D Transport test with Vacuum and Incident-isotropic BC.
-- SDM: PWLD
-- Test: Max-value=3.74343e-04
num_procs = 4





--############################################### Check num_procs
if (check_num_procs==nil and number_of_processes ~= num_procs) then
  Log(LOG_0ERROR,"Incorrect amount of processors. " ..
    "Expected "..tostring(num_procs)..
    ". Pass check_num_procs=false to override if possible.")
  os.exit(false)
end

--############################################### Setup mesh
meshgen1 = mesh.ExtruderMeshGenerator.Create
({
  inputs =
  {
    mesh.FromFileMeshGenerator.Create
    ({
      filename = "../../../../resources/TestMeshes/Square2x2_partition_cyclic3.obj"
    }),
  },
  layers = {{z=0.4,n=2},{z=0.8,n=2},{z=1.2,n=2},{z=1.6,n=2}}, -- layers
  partitioner = KBAGraphPartitioner.Create
  ({
    nx = 2, ny=2, nz=1,
    xcuts = {0.0}, ycuts = {0.0}
  })
})
mesh.MeshGenerator.Execute(meshgen1)

--############################################### Set Material IDs
vol0 = mesh.RPPLogicalVolume.Create({infx=true, infy=true, infz=true})
VolumeMesherSetProperty(MATID_FROMLOGICAL,vol0,0)

vol1 = mesh.RPPLogicalVolume.Create
({ xmin=-0.5,xmax=0.5,ymin=-0.5,ymax=0.5, infz=true })
VolumeMesherSetProperty(MATID_FROMLOGICAL,vol1,1)

--############################################### Add materials
materials = {}
materials[1] = PhysicsAddMaterial("Test Material");
materials[2] = PhysicsAddMaterial("Test Material2");

PhysicsMaterialAddProperty(materials[1],TRANSPORT_XSECTIONS)
PhysicsMaterialAddProperty(materials[2],TRANSPORT_XSECTIONS)

PhysicsMaterialAddProperty(materials[1],ISOTROPIC_MG_SOURCE)
PhysicsMaterialAddProperty(materials[2],ISOTROPIC_MG_SOURCE)


num_groups = 21
PhysicsMaterialSetProperty(materials[1],TRANSPORT_XSECTIONS,
  OPENSN_XSFILE,"xs_graphite_pure.xs")
PhysicsMaterialSetProperty(materials[2],TRANSPORT_XSECTIONS,
  OPENSN_XSFILE,"xs_graphite_pure.xs")

src={}
for g=1,num_groups do
  src[g] = 0.0
end

PhysicsMaterialSetProperty(materials[1],ISOTROPIC_MG_SOURCE,FROM_ARRAY,src)
PhysicsMaterialSetProperty(materials[2],ISOTROPIC_MG_SOURCE,FROM_ARRAY,src)

--############################################### Setup Physics
pquad0 = CreateProductQuadrature(GAUSS_LEGENDRE_CHEBYSHEV,2, 2)

lbs_block =
{
  num_groups = num_groups,
  groupsets =
  {
    {
      groups_from_to = {0, 20},
      angular_quadrature_handle = pquad0,
      --angle_aggregation_type = "single",
      angle_aggregation_num_subsets = 1,
      groupset_num_subsets = 1,
      inner_linear_method = "gmres",
      l_abs_tol = 1.0e-6,
      l_max_its = 300,
      gmres_restart_interval = 30,
    },
  }
}
bsrc={}
for g=1,num_groups do
  bsrc[g] = 0.0
end
bsrc[1] = 1.0/4.0/math.pi;
lbs_options =
{
  boundary_conditions = { { name = "zmax", type = "incident_isotropic",
                            group_strength=bsrc}},
  scattering_order = 1,
}
if (reflecting) then
  table.insert(lbs_options.boundary_conditions,
    {name = "zmax", type = "reflecting"})
end

phys1 = lbs.DiscreteOrdinatesSolver.Create(lbs_block)
lbs.SetOptions(phys1, lbs_options)

--############################################### Initialize and Execute Solver
ss_solver = lbs.SteadyStateSolver.Create({lbs_solver_handle = phys1})

SolverInitialize(ss_solver)
SolverExecute(ss_solver)

--############################################### Get field functions
fflist,count = LBSGetScalarFieldFunctionList(phys1)

--############################################### Slice plot
--slices = {}
--for k=1,count do
--    slices[k] = FFInterpolationCreate(SLICE)
--    FFInterpolationSetProperty(slices[k],SLICE_POINT,0.0,0.0,0.8001)
--    FFInterpolationSetProperty(slices[k],ADD_FIELDFUNCTION,fflist[k])
--    --FFInterpolationSetProperty(slices[k],SLICE_TANGENT,0.393,1.0-0.393,0)
--    --FFInterpolationSetProperty(slices[k],SLICE_NORMAL,-(1.0-0.393),-0.393,0.0)
--    --FFInterpolationSetProperty(slices[k],SLICE_BINORM,0.0,0.0,1.0)
--    FFInterpolationInitialize(slices[k])
--    FFInterpolationExecute(slices[k])
--    FFInterpolationExportPython(slices[k])
--end

--############################################### Volume integrations
ffi1 = FFInterpolationCreate(VOLUME)
curffi = ffi1
FFInterpolationSetProperty(curffi,OPERATION,OP_MAX)
FFInterpolationSetProperty(curffi,LOGICAL_VOLUME,vol0)
FFInterpolationSetProperty(curffi,ADD_FIELDFUNCTION,fflist[1])

FFInterpolationInitialize(curffi)
FFInterpolationExecute(curffi)
maxval = FFInterpolationGetValue(curffi)

Log(LOG_0,string.format("Max-value1=%.5e", maxval))

ffi1 = FFInterpolationCreate(VOLUME)
curffi = ffi1
FFInterpolationSetProperty(curffi,OPERATION,OP_MAX)
FFInterpolationSetProperty(curffi,LOGICAL_VOLUME,vol0)
FFInterpolationSetProperty(curffi,ADD_FIELDFUNCTION,fflist[20])

FFInterpolationInitialize(curffi)
FFInterpolationExecute(curffi)
maxval = FFInterpolationGetValue(curffi)

Log(LOG_0,string.format("Max-value2=%.5e", maxval))

--############################################### Exports
if (master_export == nil) then
  ExportFieldFunctionToVTKG(fflist[1],"ZPhi3D","Phi")

  line = FFInterpolationCreate(LINE)
  FFInterpolationSetProperty(line,LINE_FIRSTPOINT,0.0,-1.0,0.5)
  FFInterpolationSetProperty(line,LINE_SECONDPOINT,0.0, 1.0,0.5)
  FFInterpolationSetProperty(line,LINE_NUMBEROFPOINTS,1000)
  FFInterpolationSetProperty(line,ADD_FIELDFUNCTION,fflist[2])

  FFInterpolationInitialize(line)
  FFInterpolationExecute(line)

  FFInterpolationExportPython(line,"Line")
end

--############################################### Plots
if (location_id == 0 and master_export == nil) then

  --os.execute("python ZPFFI00.py")
  ----os.execute("python ZPFFI11.py")
  --local handle = io.popen("python ZPFFI00.py")
  print("Execution completed")
end