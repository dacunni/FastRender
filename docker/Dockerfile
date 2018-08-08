# Use an official Python runtime as a parent image
FROM centos:latest

# Set the working directory to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
ADD . /app

# Install any needed packages specified in requirements.txt
RUN yum install -y git make cmake \
      gcc-c++ clang \
      ImageMagick-c++-devel freeglut-devel

RUN git clone https://github.com/assimp/assimp.git && \
    cd assimp && \
    cmake CMakeLists.txt -G 'Unix Makefiles' && \
    make -j 2 install

# Update library search path
RUN echo /usr/local/lib >> /etc/ld.so.conf && ldconfig

# Install projects
RUN git clone -b linux https://github.com/dacunni/FastRender.git

# Make port 80 available to the world outside this container
EXPOSE 80

