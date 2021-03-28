FROM quay.io/pypa/manylinux2010_x86_64:2021-02-06-3d322a5

# Setup dev environment

# WORKDIR /github/workspace
ENV PLAT manylinux2010_x86_64

# Copy build script and build
COPY build_alembic.sh /entrypoint.sh

ENTRYPOINT [ "/entrypoint.sh" ]
