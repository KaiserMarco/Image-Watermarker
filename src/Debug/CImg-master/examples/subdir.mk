################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../CImg-master/examples/CImg_demo.cpp \
../CImg-master/examples/captcha.cpp \
../CImg-master/examples/curve_editor2d.cpp \
../CImg-master/examples/dtmri_view3d.cpp \
../CImg-master/examples/edge_explorer2d.cpp \
../CImg-master/examples/fade_images.cpp \
../CImg-master/examples/gaussian_fit1d.cpp \
../CImg-master/examples/generate_loop_macros.cpp \
../CImg-master/examples/hough_transform2d.cpp \
../CImg-master/examples/image2ascii.cpp \
../CImg-master/examples/image_registration2d.cpp \
../CImg-master/examples/image_surface3d.cpp \
../CImg-master/examples/jawbreaker.cpp \
../CImg-master/examples/mcf_levelsets2d.cpp \
../CImg-master/examples/mcf_levelsets3d.cpp \
../CImg-master/examples/odykill.cpp \
../CImg-master/examples/pde_TschumperleDeriche2d.cpp \
../CImg-master/examples/pde_heatflow2d.cpp \
../CImg-master/examples/plotter1d.cpp \
../CImg-master/examples/radon_transform2d.cpp \
../CImg-master/examples/scene3d.cpp \
../CImg-master/examples/spherical_function3d.cpp \
../CImg-master/examples/tetris.cpp \
../CImg-master/examples/tron.cpp \
../CImg-master/examples/tutorial.cpp \
../CImg-master/examples/use_RGBclass.cpp \
../CImg-master/examples/use_chlpca.cpp \
../CImg-master/examples/use_cimgIPL.cpp \
../CImg-master/examples/use_cimgmatlab.cpp \
../CImg-master/examples/use_draw_gradient.cpp \
../CImg-master/examples/use_jpeg_buffer.cpp \
../CImg-master/examples/use_nlmeans.cpp \
../CImg-master/examples/use_skeleton.cpp \
../CImg-master/examples/use_tiff_stream.cpp \
../CImg-master/examples/use_tinymatwriter.cpp \
../CImg-master/examples/wavelet_atrous.cpp 

OBJS += \
./CImg-master/examples/CImg_demo.o \
./CImg-master/examples/captcha.o \
./CImg-master/examples/curve_editor2d.o \
./CImg-master/examples/dtmri_view3d.o \
./CImg-master/examples/edge_explorer2d.o \
./CImg-master/examples/fade_images.o \
./CImg-master/examples/gaussian_fit1d.o \
./CImg-master/examples/generate_loop_macros.o \
./CImg-master/examples/hough_transform2d.o \
./CImg-master/examples/image2ascii.o \
./CImg-master/examples/image_registration2d.o \
./CImg-master/examples/image_surface3d.o \
./CImg-master/examples/jawbreaker.o \
./CImg-master/examples/mcf_levelsets2d.o \
./CImg-master/examples/mcf_levelsets3d.o \
./CImg-master/examples/odykill.o \
./CImg-master/examples/pde_TschumperleDeriche2d.o \
./CImg-master/examples/pde_heatflow2d.o \
./CImg-master/examples/plotter1d.o \
./CImg-master/examples/radon_transform2d.o \
./CImg-master/examples/scene3d.o \
./CImg-master/examples/spherical_function3d.o \
./CImg-master/examples/tetris.o \
./CImg-master/examples/tron.o \
./CImg-master/examples/tutorial.o \
./CImg-master/examples/use_RGBclass.o \
./CImg-master/examples/use_chlpca.o \
./CImg-master/examples/use_cimgIPL.o \
./CImg-master/examples/use_cimgmatlab.o \
./CImg-master/examples/use_draw_gradient.o \
./CImg-master/examples/use_jpeg_buffer.o \
./CImg-master/examples/use_nlmeans.o \
./CImg-master/examples/use_skeleton.o \
./CImg-master/examples/use_tiff_stream.o \
./CImg-master/examples/use_tinymatwriter.o \
./CImg-master/examples/wavelet_atrous.o 

CPP_DEPS += \
./CImg-master/examples/CImg_demo.d \
./CImg-master/examples/captcha.d \
./CImg-master/examples/curve_editor2d.d \
./CImg-master/examples/dtmri_view3d.d \
./CImg-master/examples/edge_explorer2d.d \
./CImg-master/examples/fade_images.d \
./CImg-master/examples/gaussian_fit1d.d \
./CImg-master/examples/generate_loop_macros.d \
./CImg-master/examples/hough_transform2d.d \
./CImg-master/examples/image2ascii.d \
./CImg-master/examples/image_registration2d.d \
./CImg-master/examples/image_surface3d.d \
./CImg-master/examples/jawbreaker.d \
./CImg-master/examples/mcf_levelsets2d.d \
./CImg-master/examples/mcf_levelsets3d.d \
./CImg-master/examples/odykill.d \
./CImg-master/examples/pde_TschumperleDeriche2d.d \
./CImg-master/examples/pde_heatflow2d.d \
./CImg-master/examples/plotter1d.d \
./CImg-master/examples/radon_transform2d.d \
./CImg-master/examples/scene3d.d \
./CImg-master/examples/spherical_function3d.d \
./CImg-master/examples/tetris.d \
./CImg-master/examples/tron.d \
./CImg-master/examples/tutorial.d \
./CImg-master/examples/use_RGBclass.d \
./CImg-master/examples/use_chlpca.d \
./CImg-master/examples/use_cimgIPL.d \
./CImg-master/examples/use_cimgmatlab.d \
./CImg-master/examples/use_draw_gradient.d \
./CImg-master/examples/use_jpeg_buffer.d \
./CImg-master/examples/use_nlmeans.d \
./CImg-master/examples/use_skeleton.d \
./CImg-master/examples/use_tiff_stream.d \
./CImg-master/examples/use_tinymatwriter.d \
./CImg-master/examples/wavelet_atrous.d 


# Each subdirectory must supply rules for building sources it contributes
CImg-master/examples/%.o: ../CImg-master/examples/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


