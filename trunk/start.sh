killall evremote2
evremote2 -r /root/dfb/share/evremote2/ufs912_hbbtv.rc &

export LD_LIBRARY_PATH=/root/dfb/lib:$LD_LIBRARY_PATH
ln -s /root/dfb/share/directfb-1.4.3 /usr/share/directfb-1.4.3
#ln -s /root/dfb/share/directfb-examples/ /usr/share/directfb-examples
ln -s /root/dfb/share/gtk-2.0/ /usr/share/gtk-2.0
ln -s /root/dfb/lib/directfb-1.4-0/ /usr/lib/directfb-1.4-0
ln -s /root/dfb/lib/gtk-2.0/ /usr/lib/gtk-2.0
ln -s /root/dfb/lib/pango/ /usr/lib/pango

mkdir -p /etc/pango/
#/root/dfb/bin/pango-querymodules > '/etc/pango/pango.modules'
cp /root/dfb/share/pango/pango.modules /etc/pango/
cp -r /root/dfb/share/gtk-2.0 /etc/


export DFBARGS="pixelformat=ARGB,no-cursor,bg-none"
export GTK_IM_MODULE=multipress


exec "/root/dfb/bin/$1" $2 $3 $4 $5

killall evremote2
evremote2 &
