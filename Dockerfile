# Use a specific version of GCC for reproducibility
FROM gcc:12 as builder

# Install dependencies
RUN apt-get update && apt-get install -y cmake python3.11-venv python3-pip git

WORKDIR /app

RUN python3 -m venv my-venv && my-venv/bin/pip install -U conan

RUN my-venv/bin/conan profile detect

# Modify libtool package to use ftpmirror.gnu.org since currently ftp.gnu.org is having issues
RUN git clone --depth 1 https://github.com/conan-io/conan-center-index.git /tmp/cci && \
    sed -i 's|ftp.gnu.org|ftpmirror.gnu.org|g' /tmp/cci/recipes/libtool/all/conandata.yml && \
    cd /tmp/cci/recipes/libtool/all && \
    /app/my-venv/bin/conan export . --version 2.4.7 

COPY conanfile.txt .
COPY CMakeLists.txt .
COPY src src
# Build the project
RUN my-venv/bin/conan install -of bld --build=missing .

RUN cmake --preset="conan-release" . && cmake --build --preset="conan-release"

# Create the runtime image
FROM debian:bookworm-slim 
WORKDIR /app

RUN apt-get update && apt-get install -y --only-upgrade libstdc++6

# Copy the executable from the builder stage
COPY --from=builder /app/bld/build/Release/hackathon-sample .

# Set the entrypoint
ENTRYPOINT ["./hackathon-sample"]
