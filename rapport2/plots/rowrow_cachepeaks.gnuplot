log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

L2expect(n) = (n < 180) ? (2 * n * n/8) \
                        : ((n < 1927) ? ((n*n) / 8) * (1 + n) \
                                      : (n*n*n) * (1 + 1/8))

L3expect(n) = (n < 1023) ? (2 * n * n/8) \
                         : ((n*n) / 8) * (1 + n)

set terminal pdf enhanced font "Helvetica, 10" size 7,4
set output "./rowrow_cachepeaks.pdf"

set xlabel "log2 input size"
set xtics 0, 1 rotate
set offset 1, 1

set y2label "Available cache lines"
set ylabel "Normalized cache faults"

set xrange [7.5:11.8]

set ytics nomirror tc lt 1
set y2tics nomirror tc lt 2

set logscale y
set logscale y2

set yrange [0.1:1024]
set y2range [0.1:1024]

#set logscale y2
#set grid my2tics

set key vert right top reverse
set pointsize 1

plot 'rowrow_cachepeaks.dat' using (log2($2)):($12/L2expect($2)) title "Normalized L2 cache faults" axes x1y1 with linespoints, \
     'rowrow_cachepeaks.dat' using (log2($2)):($1) title "Available cache lines" axes x1y2 with linespoints
