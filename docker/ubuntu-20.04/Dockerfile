#compile and install "antares-solver" executable
ARG url
ARG branch

FROM ubuntu:20.04

#os packages
RUN apt-get  update
RUN DEBIAN_FRONTEND=noninteractive apt-get install -yq keyboard-configuration tzdata

RUN apt-get install -y gcc-10 g++-10 cpp-10

RUN apt-get install -yq git unzip
RUN apt-get install -yq python3 python3-pip libboost-all-dev
RUN apt-get install -yq libuuid1 uuid-dev libwxgtk3.0-gtk3-dev
RUN apt-get clean -yq

#cmake.version > 3.18 for ortools
RUN pip install --upgrade cmake
RUN ln -s $HOME/.local/bin/cmake /usr/bin/cmake

RUN mkdir /opt/antares
WORKDIR /opt/antares
RUN git clone $(echo ${url:-https://github.com/AntaresSimulatorTeam/Antares_Simulator.git}) --branch $(echo ${branch:-v8.8.2}) antares_sim

#python packages
RUN pip install -r /opt/antares/antares_sim/src/tests/examples/requirements.txt

WORKDIR /opt/antares/antares_sim
RUN git submodule update --init src/antares-deps
RUN cmake -B _build -S src/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/gcc-10 -DCMAKE_CXX_COMPILER=/usr/bin/g++-10
RUN cmake --build _build --config release -j8
RUN ln -s /opt/antares/antares_sim/_build/solver/antares-8.8-solver /usr/bin/antares-solver