library(data.table)

pdf("plot.pdf", width=7.5, height=5)

d <- as.data.table(read.table("plot.data", header=T, sep=" "))

data <- copy(d)
data[ ,`:=`("Name" = NULL)]

plot(data, ylab="Area * Power", xlab="Time (Cycles)")
