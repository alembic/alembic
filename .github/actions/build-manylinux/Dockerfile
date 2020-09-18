FROM quay.io/pypa/manylinux2010_x86_64:latest

# Setup dev environment

# WORKDIR /github/workspace
ENV PLAT manylinux2010_x86_64

# Copy build script and build
COPY build_alembic.sh /entrypoint.sh

ENTRYPOINT [ "/entrypoint.sh" ]
