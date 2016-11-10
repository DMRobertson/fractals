#! /usr/bin/env Rscript

library(zoo)

time = seq(-5, 5, length.out=200)
trend = time^2
noise = rnorm(length(time), sd=1.5)
data = trend + noise

h = 5
time_sub = time[h:(length(time)+1-h)]
mov_avg = rollmean(data, 2*h-1)

xlim=range(time)
ylim=range(data)

pdf("processed/time_series.pdf")
par(xaxt="n", yaxt="n", ann=F, bty="n", mai=rep(0, 4))
plot(time, data, type='l', xlim=xlim, ylim=ylim)

pdf("processed/moving_average.pdf", )
par(xaxt="n", yaxt="n", ann=F, bty="n", mai=rep(0, 4))
plot(time_sub, mov_avg, type='l', col="blue", xlim=xlim, ylim=ylim)

pdf("processed/trend.pdf")
par(xaxt="n", yaxt="n", ann=F, bty="n", mai=rep(0, 4))
plot(time, trend, type='l', col="red", xlim=xlim, ylim=ylim)
dev.off()
