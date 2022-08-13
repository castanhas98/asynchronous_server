# rm -r build

mkdir build

cd build || {
  echo 'Entry into build folder failed.'
  exit 1
}

cmake -DCMAKE_PREFIX_PATH="${CONDA_PREFIX}" \
      ..

cmake --build . # -- -j 4

