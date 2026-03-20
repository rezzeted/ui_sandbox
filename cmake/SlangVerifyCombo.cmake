# Invoked: cmake -DOUT=... -DVERTEX=... -DCOMMON=... -DFRAG=... -DSLANGC=... -DUSE_COMMON=TRUE|FALSE -P SlangVerifyCombo.cmake
cmake_minimum_required(VERSION 3.21)

file(READ "${VERTEX}" _v)
if(USE_COMMON STREQUAL "1")
  file(READ "${COMMON}" _c)
else()
  set(_c "")
endif()
file(READ "${FRAG}" _f)
file(WRITE "${OUT}" "${_v}\n${_c}\n${_f}\n")

execute_process(
  COMMAND "${SLANGC}" "${OUT}" -entry vertexMain -stage vertex -target glsl -o "${OUT}.vertex.glsl"
  RESULT_VARIABLE _rv
)
if(_rv)
  message(FATAL_ERROR "slangc vertex failed (${_rv}) for ${OUT}")
endif()
execute_process(
  COMMAND "${SLANGC}" "${OUT}" -entry fragmentMain -stage fragment -target glsl -o "${OUT}.fragment.glsl"
  RESULT_VARIABLE _rf
)
if(_rf)
  message(FATAL_ERROR "slangc fragment failed (${_rf}) for ${OUT}")
endif()
