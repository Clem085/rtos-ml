set terminal pngcairo  transparent enhanced font "arial,10" fontscale 1.0 size 1600,400
set output 'gantt.png'
#set term x11 size 1600,400
set title "Gantt Chart"
#set xdata #time
#set format x "%b\n'%y" timedate
#timeformat = "%f"
set xlabel "Time"
set ylabel "Task"
#set xrange [1:20]
set yrange [0:10]
#set xtics 1
set ytics 1
set style rectangle
unset key
plot "gantt.dat" using 2:1:($3-$2):(0) with vector nohead lw 5
#pause -1 "Hit any key to continue"
