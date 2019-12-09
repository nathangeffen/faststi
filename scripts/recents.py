from enum import Enum, IntEnum
import argparse
import json
import numpy
import math
import matplotlib
from multiprocessing import Pool, cpu_count
import pandas
import random
import statistics
import sys
import time

###############
# Parameters  #
###############

PROB_MALE = 0.5
PROB_LO = 0.5

LO_STAGE_UNINFECTED = 0.9999
LO_STAGE_TREATED = 0.99995
LO_STAGE_PRIMARY = 0.99996
LO_STAGE_CHRONIC = 0.99999

HI_STAGE_UNINFECTED = 0.999
HI_STAGE_TREATED = 0.9995
HI_STAGE_PRIMARY = 0.9996
HI_STAGE_CHRONIC = 0.9999

# Parameters for number of days till single agent enters mating pool
LO_SINGLE_TO_MATING_MEAN = 150.0
LO_SINGLE_TO_MATING_SD = 100.0
HI_SINGLE_TO_MATING_MEAN = 10.0
HI_SINGLE_TO_MATING_SD = 5.0

# Parameters for number of days till paired agents becomes single
LO_MATING_TO_SINGLE_MEAN = 2.0 * 365.0
LO_MATING_TO_SINGLE_SD = 300.0
HI_MATING_TO_SINGLE_MEAN = 10.0
HI_MATING_TO_SINGLE_SD = 5.0

PROB_PRIMARY_TRACE_SUCCESS = 0.8
PROB_SECONDARY_TRACE_SUCCESS = 0.6

PRIMARY_TO_CHRONIC = 0.04
CHRONIC_TO_SICK = 0.0004

# Probabilities of getting tested (per day)
PROB_TEST_MALE_STAGE_0 = 0.001
PROB_TEST_MALE_STAGE_1 = 0.0
PROB_TEST_MALE_STAGE_2 = 0.0015
PROB_TEST_MALE_STAGE_3 = 0.001
PROB_TEST_MALE_STAGE_4 = 0.01

PROB_TEST_FEMALE_STAGE_0 = 0.002
PROB_TEST_FEMALE_STAGE_1 = 0.0
PROB_TEST_FEMALE_STAGE_2 = 0.003
PROB_TEST_FEMALE_STAGE_3 = 0.002
PROB_TEST_FEMALE_STAGE_4 = 0.01


PROB_TEST_TABLE = [
    [
        PROB_TEST_MALE_STAGE_0,
        PROB_TEST_MALE_STAGE_1,
        PROB_TEST_MALE_STAGE_2,
        PROB_TEST_MALE_STAGE_3,
        PROB_TEST_MALE_STAGE_4
    ],
    [
        PROB_TEST_FEMALE_STAGE_0,
        PROB_TEST_FEMALE_STAGE_1,
        PROB_TEST_FEMALE_STAGE_2,
        PROB_TEST_FEMALE_STAGE_3,
        PROB_TEST_FEMALE_STAGE_4
    ]
]

CSV_COLUMNS = ["proc", "iteration", "stage", "infected",
               "treated", "paired"]

################
# Enumerations #
################

class Sex(IntEnum):
    MALE = 0
    FEMALE = 1

class Risk(Enum):
    LO = 0
    HI = 1

class Stage(IntEnum):
    UNINFECTED = 0
    TREATED = 1
    PRIMARY = 2
    CHRONIC = 3
    SICK = 4

INITIAL_MATING_TABLE =  {
    Sex.MALE: {
        Risk.LO: 0.4,
        Risk.HI: 0.6
    },
    Sex.FEMALE: {
        Risk.LO: 0.4,
        Risk.HI: 0.6
    }
}

INFECT_RISK_TABLE = {
    Sex.MALE: {
        Sex.MALE: {
            Stage.PRIMARY: 0.1,
            Stage.CHRONIC: 0.02,
            Stage.SICK: 0.02
        },
        Sex.FEMALE: {
            Stage.PRIMARY: 0.02,
            Stage.CHRONIC: 0.04,
            Stage.SICK: 0.04
        },
    },
    Sex.FEMALE: {
        Sex.MALE: {
            Stage.PRIMARY: 0.048,
            Stage.CHRONIC: 0.02,
            Stage.SICK: 0.02
        },
        Sex.FEMALE: {
            Stage.PRIMARY: 0.0008,
            Stage.CHRONIC: 0.0006,
            Stage.SICK: 0.0006
        },
    }
}

class Agent:

    def __init__(self, iden):
        self.iden = iden
        self.sex = Sex.MALE if random.random() < PROB_MALE else Sex.FEMALE
        self.risk = Risk.LO if random.random() < PROB_LO else Risk.HI
        self.partner = None
        self.last_partner = None
        self.breakup_iter = None
        self.infection_iter  = None
        self.init_infected()
        self.rel_change = None

    def init_infected(self):
        r = random.random()
        if self.risk == Risk.LO:
            if r < LO_STAGE_UNINFECTED:
                self.infected = Stage.UNINFECTED
            elif r < LO_STAGE_TREATED:
                self.infected = Stage.TREATED
            elif r < LO_STAGE_PRIMARY:
                self.infected = Stage.PRIMARY
            elif r < LO_STAGE_CHRONIC:
                self.infected = Stage.CHRONIC
            else:
                self.infected = Stage.SICK
        else:
            if r < HI_STAGE_UNINFECTED:
                self.infected = Stage.UNINFECTED
            elif r < HI_STAGE_TREATED:
                self.infected = Stage.TREATED
            elif r < HI_STAGE_PRIMARY:
                self.infected = Stage.PRIMARY
            elif r < HI_STAGE_CHRONIC:
                self.infected = Stage.CHRONIC
            else:
                self.infected = Stage.SICK
        if self.infected > Stage.UNINFECTED:
            self.infection_iter = -10000000


class Simulation:

    def __init__(self, parameters = {}):
        # Initialize outputs
        self.iteration = 0
        self.recents = 0
        self.primary_traces = 0
        self.secondary_traces = 0
        self.breakups = 0
        self.sim_pairs = 0
        self.infections = 0
        self.poss_inf = 0
        self.report_table = []

        # Parameters
        self.proc_no = parameters["proc_no"] if "proc_no" in parameters else 0
        self.num_agents = parameters["num_agents"] \
                          if "num_agents" in parameters else 20000
        self.burn_in_iterations = parameters["burn_in_iterations"] \
                          if "burn_in_iterations" in parameters else 3650
        self.iterations = parameters["iterations"] \
                          if "iterations" in parameters else 1461 + \
                             self.burn_in_iterations
        self.before_events = parameters["before_events"] \
                        if "before_events" in parameters \
                           else [self.print_report_header,
                                 self.init_pairs,
                                 self.init_singles]
        self.burn_in_events = parameters["burn_in_events"] \
                        if "burn_in_events" in parameters else \
                           [self.pair,
                            self.infect,
                            self.breakup,
                            self.advance,
                            self.report]
        self.during_events = parameters["during_events"] \
                        if "during_events" in parameters else \
                           [self.pair,
                            self.infect,
                            self.breakup,
                            self.get_tested,
                            self.advance,
                            self.report]
        self.after_events = parameters["after_events"] \
                        if "after_events" in parameters else []
        self.match_k = parameters["match_k"] \
                       if "match_k" in parameters else 100
        self.trace_partners = parameters["trace_partners"] \
                        if "trace_partners" in parameters else True
        self.cutoff = parameters["cutoff"] \
                      if "cutoff" in parameters else 60
        self.agents = parameters["agents"] \
                      if "agents" in parameters \
                         else [Agent(i) for i in range(0, self.num_agents)]


    def print_report_header(self):
        if self.proc_no == 0:
            print(",".join(CSV_COLUMNS))

    def init_pairs(self):
        pool = [a for a in self.agents
                if random.random() < INITIAL_MATING_TABLE[a.sex][a.risk]]
        self.match(pool)
        for agent in pool[0:(len(pool) // 2)]:
            agent.rel_change = random.random() * agent.rel_change
            self.agents[agent.partner].rel_change = agent.rel_change

    def init_singles(self):
        for agent in [a for a in self.agents
                      if a.partner is None]:
            self.set_single_len(agent)
            agent.rel_change = random.random() * agent.rel_change

    def infect(self):
        pool = [(a, self.agents[a.partner], random.random())
                for a in self.agents if
                a.infected == Stage.UNINFECTED and
                a.partner is not None and
                self.agents[a.partner].infected > Stage.TREATED]
        self.poss_inf = self.poss_inf + len(pool)
        for agent, partner, r in pool:
            if r < INFECT_RISK_TABLE[agent.sex][partner.sex][partner.infected]:
                agent.infected = Stage.PRIMARY
                agent.infection_iter = self.iteration
                self.infections = self.infections + 1

    def is_recent(self, agent):
        return True if (self.iteration - agent.infection_iter <
                            self.cutoff) else False

    def trace(self, agent):
        if self.is_recent(agent):
            self.recents = self.recents + 1
            if agent.partner is not None:
                partner = self.agents[agent.partner]
                if random.random() < PROB_PRIMARY_TRACE_SUCCESS:
                    if partner.infected > Stage.TREATED:
                        partner.infected = Stage.TREATED
                        self.primary_traces = self.primary_traces + 1

            if agent.last_partner is not None:
                if self.iteration - agent.breakup_iter < self.cutoff:
                    partner = self.agents[agent.last_partner]
                    if random.random() < PROB_SECONDARY_TRACE_SUCCESS:
                        if partner.infected > Stage.TREATED:
                            partner.infected = Stage.TREATED
                            self.secondary_traces = self.secondary_traces + 1

    def get_tested(self):
        for agent in [a for a in self.agents if a.infected > Stage.TREATED]:
            if random.random() < PROB_TEST_TABLE[agent.sex][agent.infected]:
                agent.infected = Stage.TREATED
                if self.trace_partners:
                    self.trace(agent)

    def advance(self):
        for agent in [a for a in self.agents if a.infected > Stage.TREATED]:
            if agent.infected == Stage.PRIMARY:
                if random.random() < PRIMARY_TO_CHRONIC:
                    agent.infected = Stage.CHRONIC
            elif agent.infected == Stage.CHRONIC:
                if random.random() < CHRONIC_TO_SICK:
                    agent.infected = Stage.SICK

    def pair(self):
        pool = [a for a in self.agents
                if a.partner is None and self.iteration > a.rel_change]
        self.match(pool)
        self.sim_pairs = self.sim_pairs + int(len(pool) / 2)

    def breakup(self):
        pool = set([min(a.iden, a.partner) for a in self.agents
                    if a.partner is not None and self.iteration > a.rel_change])
        self.breakups = self.breakups + len(pool)
        for iden in pool:
            a = self.agents[iden]
            b = self.agents[a.partner]
            a.breakup_iter = self.iteration
            b.breakup_iter = self.iteration
            a.last_partner = b.iden
            b.last_partner = a.iden
            a.partner = None
            b.partner = None
            self.set_single_len(a)
            self.set_single_len(b)

    def distance(self, a, b):
        d = 100 if a.sex == b.sex else 0
        return (d + 50) if a.risk == b.risk else d

    def pair_quality(self):
        bad = []
        dist = 0.0
        unusual_0 = 0
        unusual_1 = 0
        unusual_2 = 0
        pool = [a for a in self.agents if a.partner is not None]
        if (len(pool) == 0):
            return None
        for agent in pool:
            index = agent.partner
            partner = self.agents[index]
            if partner.partner != agent.iden:
                bad.append((agent.iden, partner.iden, "mismatch",))
            elif agent.iden < partner.iden:
                d = self.distance(agent, partner)
                if d > 0:
                   unusual_0 = unusual_0 + 1
                if d > 50:
                    unusual_1 = unusual_1 + 1
                if d > 100:
                    unusual_2 = unusual_2 + 1
                dist = dist + self.distance(agent, partner)
        return {"pairs": len(pool) / 2,
                "avg distance": dist / len(pool) / 2,
                "unusual_0": unusual_0,
                "unusual_1": unusual_1,
                "unusual_2": unusual_2,
                "corrupt": bad}

    def set_single_len(self, agent):
        m = LO_SINGLE_TO_MATING_MEAN if agent.risk == Risk.LO \
            else HI_SINGLE_TO_MATING_MEAN
        s = LO_SINGLE_TO_MATING_SD if agent.risk == Risk.LO \
            else HI_SINGLE_TO_MATING_SD
        agent.rel_change = self.iteration + int(random.normalvariate(m, s))

    def set_rel_len(self, a, b):
        m1 = LO_MATING_TO_SINGLE_MEAN if a.risk == Risk.LO \
             else HI_MATING_TO_SINGLE_MEAN
        m2 = LO_MATING_TO_SINGLE_MEAN if b.risk == Risk.HI \
             else HI_MATING_TO_SINGLE_MEAN
        s1 = LO_MATING_TO_SINGLE_SD if a.risk == Risk.LO \
             else HI_MATING_TO_SINGLE_SD
        s2 = LO_MATING_TO_SINGLE_SD if b.risk == Risk.HI \
             else HI_MATING_TO_SINGLE_SD
        a.rel_change = b.rel_change = self.iteration + \
                                      int(random.normalvariate(m1 + m2, s1 + s2))


    def match(self, pool):
        k = self.match_k
        random.shuffle(pool)
        n = len(pool)
        back = n - 1
        for i in range(0, int(n // 2)):
            j = i + 1
            best_dist = 999999
            best_index = -1
            while j < i + k and j <= back:
                distance = self.distance(pool[i], pool[j])
                if  distance < best_dist:
                    best_dist = distance
                    best_index = j
                j = j + 1
            pool[best_index].partner = pool[i].iden
            pool[i].partner = pool[best_index].iden
            self.set_rel_len(pool[i], pool[best_index])
            t = pool[back]
            pool[back] = pool[best_index]
            pool[best_index] = t
            back = back - 1

    def report(self):
        self.report_table.append( (self.proc_no,
                                   self.iteration,
                                   "Burn-in" if
                                   self.iteration < self.burn_in_iterations
                                   else "During",
                                   len([a for a in self.agents
                                        if a.infected > Stage.UNINFECTED]),
                                   len([a for a in self.agents
                                         if a.infected == Stage.TREATED]),
                                   len([a for a in self.agents
                                        if a.partner is not None])))
        if self.iteration % 100 == 0:
            print(",".join([str(field) for field in self.report_table[-1]]))

    def run(self):
        for event in self.before_events:
            event()

        for self.iteration in range(self.iteration, self.burn_in_iterations):
            for event in self.burn_in_events:
                event()

        for self.iteration in range(self.iteration, self.iterations):
            for event in self.during_events:
                event()

        for event in self.after_events:
            event()

    def plot(self, col="infected"):
        df = pandas.DataFrame(self.report_table, columns=CSV_COLUMNS)
        start = self.burn_in_iterations
        series = pandas.Series(df[col][start:], index=df["iteration"][start:])
        series.plot.line()
        matplotlib.pyplot.show()

def run(parameters={}):
    proc_no = parameters["proc_no"] if "proc_no" in parameters else 0
    s = Simulation(parameters)
    start = time.time()
    s.run()
    end = time.time()
    s.time_taken = end - start
    return s

def multi_threaded_run(parameter_list=[{}],
                       simulations=4,
                       threads=cpu_count()):

    if len(parameter_list) == 1:
        parameter_list = [parameter_list[0].copy() for _ in range(simulations)]
    else:
        parameter_list = [p.copy() for p in parameter_list]

    if len(parameter_list) != simulations:
        print("Warning: setting num simulations to match len of parameter list",
              file=sys.stderr)
        simulations = len(parameter_list)
    i = 0
    for parameters in parameter_list:
        parameters["proc_no"] = i
        i = i + 1

    results = []
    for i in range(simulations // threads + 1):
        parameters = parameter_list[i * threads: i * threads + threads]
        with Pool(threads) as p:
            results = results + p.map(run, parameters)

    return results

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Run simulations of "
                                     "recent infection follow-ups.")
    parser.add_argument('--simulations', '-s', required=False,
                        help='Number of simulations to run', default=2)
    parser.add_argument('--parameter_list', '-p', required=False,
                        help='Parameters for simulations', default="[{}]")
    args = parser.parse_args()
    results = multi_threaded_run(parameter_list=json.loads(args.parameter_list),
                                 simulations=int(args.simulations))
    for r in results:
        r.plot()
