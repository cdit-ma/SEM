# Build Deps
yum install -y \
  epel-release \
  git \
  wget &&\
yum install -y centos-release-scl && \
yum install -y java-1.8.0-openjdk \
  chrony \
  devtoolset-7 \
  yum-utils \
  device-mapper-persistent-data \
  lvm2 \
  ninja-build \
  cmake3 \
  ccache \
  python-devel && \
yum-config-manager --add-repo https://download.docker.com/linux/centos/docker-ce.repo && \
yum install -y docker-ce docker-ce-cli containerd.io && \
yum install -y https://download.postgresql.org/pub/repos/yum/reporpms/EL-7-x86_64/pgdg-redhat-repo-latest.noarch.rpm && \
yum install -y postgresql10-devel && \
yum install -y libcurl-devel && \
ln -s /usr/bin/cmake3 /usr/bin/cmake && \
. /opt/rh/devtoolset-7/enable && \

# Install Boost
wget https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.gz -q && \
tar xf boost_1_67_0.tar.gz && \
pushd boost_1_67_0 && \
./bootstrap.sh && \
./b2 -j6 -d0 && \
./b2 install -d0 && \
popd && \
rm -rf boost_1_67_0.tar.gz boost_1_67_0 && \

# Setup Jenkins Directory
sudo mkdir /mnt/Jenkins && \
sudo chown cdit-ma /mnt/Jenkins/

# TODO: Warn on missing chrony install
