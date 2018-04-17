FROM ubuntu:xenial

# install packages due to https://github.com/obsproject/obs-studio/wiki/Install-Instructions#debian-based-build-directions
RUN apt-get update && \
  apt-get install -y build-essential pkg-config cmake git-core checkinstall \
          libx11-dev libgl1-mesa-dev libvlc-dev libpulse-dev libxcomposite-dev \
          libxinerama-dev libv4l-dev libudev-dev libfreetype6-dev \
          libfontconfig-dev qtbase5-dev libqt5x11extras5-dev libx264-dev \
          libxcb-xinerama0-dev libxcb-shm0-dev libjack-jackd2-dev libcurl4-openssl-dev

# install ffmpeg
RUN apt-get install -y libavcodec-dev libavfilter-dev libavdevice-dev libfdk-aac-dev

# Building and installing OBS:
RUN git clone --recursive https://github.com/obsproject/obs-studio.git
WORKDIR /obs-studio

# Adding my code
COPY add_cheadless_target.patch .
RUN patch < add_cheadless_target.patch
COPY CHEADLESS ./CHEADLESS
RUN mkdir build && cd build && \
  cmake -DUNIX_STRUCTURE=1 -DCMAKE_INSTALL_PREFIX=/usr .. && \
  make -j4

# running
RUN ln -s /obs-studio/build/rundir/RelWithDebInfo/data /usr/share/obs
RUN mkdir /usr/share/obs/plugin_config_info
WORKDIR /obs-studio/build/rundir/RelWithDebInfo/bin/64bit
RUN ln -s libobs-opengl.so.0.0 libobs-opengl.so.0
CMD ./obs-cheadless

## build:   docker build . -t obs_proof
## run:     docker run --rm -it -e DISPLAY=:0 -v /tmp/.X11-unix:/tmp/.X11-unix obs_proof

# падает для разрешения больше и меньше разрешения экрана
# падает при gpu_conversion = true и gpu_conversion = false
# падает и в голом X и в xfce
