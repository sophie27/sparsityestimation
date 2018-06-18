set terminal postscript enhanced color 
set output 'G.eps'
set cbrange [0:1]
unset key
#unset colorbox 
set xrange [0:12500]
set yrange [0:12500]
set xlabel "{/Times-Italic m}"  font ",30"
set ylabel "{/Times-Italic k}"  font ",30"

plot 'kmG.txt'  using 2:1:3 with image,\
x lc 1








