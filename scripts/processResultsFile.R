# Takes an results CSV file produced by FastSTI
# and runs a function (default mean) over a
# statistic (description) grouping the result 
# either by the maximum date or all the dates.

processFastSTIResults <- function(filename="results.csv", 
                                  sep=";", 
                                  description="INFECT_RATE_ALIVE",
                                  fun=mean,
                                  header=TRUE,
                                  filter_max_date=TRUE) {
  inp = read.csv(filename, sep=sep, header=header)
  values = inp[inp$description==description,]
  if (filter_max_date) {
    analysisDate = max(as.Date(values$date))
    values = values[as.Date(values$date)==analysisDate,]
  }
  mean_values = aggregate(values$value, by=list(values$name,values$date),FUN=fun)
}

print(processFastSTIResults())

