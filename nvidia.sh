sudo apt-get remove --purge '^nvidia-.*'

sudo add-apt-repository ppa:graphics-drivers/ppa
sudo apt-get update

# blacklist nouveau nvidia driver
sudo nano /etc/modprobe.d/blacklist.conf
# add following to end of file
blacklist nouveau

# Update the initramfs by entering the following command
sudo update-initramfs -u


sudo reboot

apt-cache search nvidia-driver


Download the NVIDIA driver (e.g., 525.89.02) from the NVIDIA website: https://www.nvidia.com/Download/index.aspx
Make the downloaded installer executable:
chmod +x NVIDIA-Linux-x86_64-525.89.02.run

press ctrl + alt + F2/F3/F4/...

sudo systemctl stop lightdm
sudo systemctl stop gdm
sudo systemctl stop sddm

sudo apt install gcc && make

sudo ./NVIDIA-Linux-x86_64-525.89.02.run

sudo reboot
