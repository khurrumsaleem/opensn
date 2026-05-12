# OpenSn Docker Image

This directory contains a Dockerfile for building OpenSn in an Ubuntu-based
container.

## Build

From the repository root:

```bash
docker build -f distribution/docker/Dockerfile -t opensn:latest .
```

To control the number of build jobs used inside the image:

```bash
docker build \
  -f distribution/docker/Dockerfile \
  --build-arg NUM_CORES=8 \
  -t opensn:latest .
```

## Run

Start an interactive shell:

```bash
docker run --rm -it opensn:latest /bin/bash
```

Mount a local working directory at `/work`:

```bash
docker run --rm -it -v "$PWD:/work" opensn:latest /bin/bash
```

The image builds OpenSn from the upstream GitHub repository and places a
working directory at `/work`.

## Contents

The Dockerfile installs the system build tools, builds OpenSn dependencies into
`/opensn/tools/tpls`, builds OpenSn, installs the Python package, and includes
common Python packages for analysis and visualization.

## Notes

- The image is intended as a convenient reproducible build and runtime
  environment.
- For source-tree development, mount your checkout into `/work` and build there,
  or adapt the Dockerfile to copy a local checkout instead of cloning from
  GitHub.
- For package-managed installs outside a container, see
  `distribution/spack/README.md`.
