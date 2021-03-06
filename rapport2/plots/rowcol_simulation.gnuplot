log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

L2expect(n) = (n < 180) ? (2 * n * n/8) \
                        : (n * n / 8) * (1 + n)

L3expect(n) = (n < 1023) ? (2 * n * n/8) \
                         : ((n*n) / 8) * (1 + n)

set terminal pdf enhanced font "Helvetica, 10" size 7,4
set output "./rowcol_simulation.pdf"

set xlabel "log2 input size"
set xtics 0, 1 rotate
set offset 1, 1

set ylabel "Running time [s]"
set y2label "Cache faults"

set ytics nomirror tc lt 1
set y2tics nomirror tc lt 2

set logscale y
set logscale y2
set grid my2tics

set key vert left top reverse
set pointsize 1

plot 'rowcol_foo.dat' using (log2($1)):($7) title "Time" axes x1y1 with linespoints, \
     'rowcol_foo.dat' using (log2($1)):($11) title "L2 cache faults" axes x1y2 with linespoints, \
     'rowcol_foo.dat' using (log2($1)):($13) title "L3 cache faults" axes x1y2 with linespoints, \
     'rowcol_sim.dat' using (log2($1)):($2) title "L2 Simulation" axes x1y2 with linespoints, \
     'rowcol_sim.dat' using (log2($1)):($3) title "L2 Simulation + loop" axes x1y2 with linespoints, \
     L2expect(2 ** x) title "L2 cache faults expectation" axes x1y2, \
     L3expect(2 ** x) title "L3 cache faults expectation" axes x1y2
