"""Takes a results CSV file produced by FastSTI and runs a function (default
mean) over a statistic (description) grouping the result either by the maximum
date or all the dates.

"""

import pandas

def processFastSTIResults(filename="results.csv",
                          sep=";",
                          description="INFECT_RATE_ALIVE",
                          fun=pandas.DataFrame.mean,
                          filter_max_date=True):
    df = pandas.read_csv(filename, sep=sep)
    values = df[df["description"] == description]
    if filter_max_date:
        analysisDate = df["date"].max()
        values = values[df["date"] == analysisDate]
    return values.groupby("name").apply(fun)

print(processFastSTIResults())
