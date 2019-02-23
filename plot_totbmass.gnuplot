set terminal postscript eps color enhanced "Times-Roman" 24
set size 2.2,2.2
set key inside
set xrange [0:40]
set xlabel "time"
set ylabel "Total Biomass" 
set yrange [0.0:1.1]
set xtics 0,2,40
set tics out nomirror
xi = "0.03 0.04 0.05 0.06 0.07 0.08 0.09 0.10 0.20 0.30"

do for [i in xi] {
	outfile = sprintf('totbiomass_xi=%s.eps',i)
    set output outfile
    set title "Mean, mean + one standard deviation and mean - one standard deviation of total biomass vs time for 5 seeds, N=256, xi=".i
    infile = sprintf('Spatially_integrated_bmassdns_256x256_xi-%s',i)
    plot infile u 1:2 ti 'mean', infile u 1:3 ti 'mean + one standard deviation', infile u 1:4 ti 'mean - one standard deviation'
}
