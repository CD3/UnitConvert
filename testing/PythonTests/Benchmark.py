import timeit,time,math

class Measurement:
  def __init__(self,nom,err):
    self.nominal = nom
    self.error = err
  
  @property
  def upper(self):
    return self.nominal + self.error

  @property
  def lower(self):
    return self.nominal - self.error

  def __str__(self):
    return f"{self.nominal} +/- {self.error}"

class Stats:
  def __init__(self):
    self._count = 0
    self._mean = None
    self._min = None
    self._max = None

    self._stddev = None
    self._M2 = None

  def add(self, val):
    if self._count == 0:
      val = float(val)
      self._count = 1
      self._mean = val
      self._min  = val
      self._max  = val
      self._M2 = 0
      return

    self._count += 1
    if val > self._max:
      self._max = val
    if val < self._min:
      self._min = val

    delta = val - self._mean
    self._mean = self._mean + delta / self._count
    delta2 = val - self._mean
    self._M2 = self._M2 + delta * delta2

  @property
  def mean(self):
    return self._mean

  @property
  def min(self):
    return self._min

  @property
  def max(self):
    return self._max

  @property
  def count(self):
    return self._count

  @property
  def variance(self):
    return self._M2 / self._count

  @property
  def stddev(self):
    return self.variance**0.5

  @property
  def error(self):
    return self.stddev / self._count**0.5





class Benchmark:
  def __init__(self):
    self.timer = timeit.default_timer
    self.stats = Stats()

    self.burn_iters = 10
    self.min_iters = 10
    self.max_iters = 1000
    self.max_runtime = 10
    self.tol = 0.01

  def record(self,f,*args,**kwargs):
    start = self.timer()
    value = f(*args,**kwargs)
    end = self.timer()
    duration = end-start
    self.stats.add(duration)


  def run(self,f,*args,**kwargs):
    self.reset()

    for i in range(self.burn_iters):
      f(*args,**kwargs)

    while( self.stats.count < self.min_iters or (self.stats.count < self.max_iters
      and self.stats.error/self.stats.mean > self.tol) ):
      self.record(f,*args,**kwargs)

  def reset(self):
    self.stats = Stats()
  
  @property
  def measurement(self):
    return Measurement(self.stats.mean, self.stats.error)



def Speedup(m1, m2):
  '''Compute the "speedup" for measurment 'm2' over measurment 'm1'.'''

  snom = m1.nominal/m2.nominal
  serr1 = m1.upper/m2.nominal - snom
  serr2 = m1.nominal/m2.upper - snom
  serr = (serr1**2 + serr2**2)**0.5

  m = Measurement(snom,serr)

  return m
