FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    g++ cmake git libboost-all-dev libssl-dev \
    libcurl4-openssl-dev nlohmann-json3-dev

# Install CPR
RUN git clone --recurse-submodules https://github.com/whoshuu/cpr.git /cpr \
    && cd /cpr \
    && mkdir build && cd build \
    && cmake .. -DCPR_FORCE_USE_SYSTEM_CURL=ON \
    && make && make install

# Set the working directory for your application
WORKDIR /app

# Copy the project files
COPY . .

# Create build directory
RUN mkdir build

# Change to build directory and run cmake
WORKDIR /app/build
RUN cmake .. && make

# Command to run the client
CMD ["./molly_api_client"]
