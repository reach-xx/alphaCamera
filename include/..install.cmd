cmd_/home/sying/wucaiyuan_toolchain/v400/glibc_gcc4.8_linaro_toolchain/linux-2012.09/tmp-install/include/.install := perl scripts/headers_install.pl /home/sying/wucaiyuan_toolchain/v400/glibc_gcc4.8_linaro_toolchain/linux-2012.09/include /home/sying/wucaiyuan_toolchain/v400/glibc_gcc4.8_linaro_toolchain/linux-2012.09/tmp-install/include arm ; perl scripts/headers_install.pl /home/sying/wucaiyuan_toolchain/v400/glibc_gcc4.8_linaro_toolchain/linux-2012.09/include /home/sying/wucaiyuan_toolchain/v400/glibc_gcc4.8_linaro_toolchain/linux-2012.09/tmp-install/include arm ; perl scripts/headers_install.pl /home/sying/wucaiyuan_toolchain/v400/glibc_gcc4.8_linaro_toolchain/linux-2012.09/include /home/sying/wucaiyuan_toolchain/v400/glibc_gcc4.8_linaro_toolchain/linux-2012.09/tmp-install/include arm ; for F in ; do echo "\#include <asm-generic/$$F>" > /home/sying/wucaiyuan_toolchain/v400/glibc_gcc4.8_linaro_toolchain/linux-2012.09/tmp-install/include/$$F; done; touch /home/sying/wucaiyuan_toolchain/v400/glibc_gcc4.8_linaro_toolchain/linux-2012.09/tmp-install/include/.install
