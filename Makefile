KDIR_HOST = $(HOME)/Kernels/linux-rel_imx_4.1.15_2.1.0_ga
KDIR_SOM = /home/dmitry/source/kernel/linux-rel_imx_4.1.15_2.1.0_ga
ARCH = arm
CCFLAGS = -C
COMPILER = arm-linux-gnueabihf-
PWD = $(shell pwd)
TARGET_MOD = button
REMFLAGS = -g -O0

# Опция -g - помещает в объектный или исполняемый файл информацию необходимую для 
# работы отладчика gdb. При сборке какого-либо проекта с целью последующей отладки, 
# опцию -g необходимо включать как на этапе компиляции так и на этапе компоновки.

# Опция -O0 - отменяет какую-либо оптимизацию кода. Опция необходима на этапе 
# отладки приложения. Как было показано выше, оптимизация может привести к 
# изменению структуры программы до неузнаваемости, связь между исполняемым и 
# исходным кодом не будет явной, соответственно, пошаговая отладка программы 
# будет не возможна. При включении опции -g, рекомендуется включать и -O0.

obj-m   := $(TARGET_MOD).o
CFLAGS_$(TARGET_MOD).o := -DDEBUG

comp_som:
	@echo "Compiling on SomLabs"
	$(MAKE) $(CCFLAGS) $(KDIR_SOM) M=$(PWD) modules CFLAGS_MODULE=-fno-pic
comp_host:
	@echo "Compiling on Host"
	$(MAKE) $(CCFLAGS) $(KDIR_HOST) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(COMPILER) modules
dtb_som:
	./compile.sh
copy_som:
	@echo "Copy my driver to ../gpio"
	@cp /home/dmitry/modules/SomLabs-Button/button.ko /lib/modules/4.1.15/kernel/drivers/gpio/
compile_dtb:
	./CompileDTB.sh
copy_dtb:
	./CopyDTB.sh
copy_mod:
	./CopyMod.sh
clean:
	@rm -f *.ko
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order *.dwo *.mod.dwo .*.dwo
	@rm -f .*.*.cmd *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions
	@rm -f *.symversclear
