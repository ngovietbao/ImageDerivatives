cmake_minimum_required(VERSION 2.8)
project( ImageDerivatives )
find_package( OpenCV REQUIRED )
add_executable( Source Source.cpp ImageProccessing.h ImageProccessing.cpp HoughTransform.h HoughTransform.cpp  HoughTransformRound.cpp GeometricTransform.h GeometricTransform.cpp FourierTransformation.h FourierTransformation.cpp ImageEnhancement.cpp ImageEnhancement.h)
target_link_libraries( Source ${OpenCV_LIBS})
