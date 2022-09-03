echo "Installing required packages..."
sudo apt-get install make nasm gcc xorriso qemu qemu-system-x86 qemu-system-i386 virtualbox mtools

echo "Compiling code..."
make

echo "Running QEMU..."
./StartQEMU.sh

echo "DONE."