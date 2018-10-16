import UnitConvert as uc
import pint
import timeit,time,math




def Close(x,y,tol=0.01):
  return math.fabs(x-y) <= tol*math.fabs(x)


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

  def get_measurement(self):
    pass

def test_stats():
  s = Stats()

  s.add(1)
  assert Close(s.count, 1)
  assert Close(s.mean, 1)
  assert Close(s.min, 1)
  assert Close(s.max, 1)
  assert Close(s.stddev, 0)
  assert Close(s.variance, 0)
  assert Close(s.error, 0)

  s.add(2)
  assert Close(s.count, 2)
  assert Close(s.mean, 1.5)
  assert Close(s.min, 1)
  assert Close(s.max, 2)
  assert Close(s.stddev, (2*0.5**2/2)**0.5 )
  assert Close(s.error,  (2*0.5**2/2)**0.5/2**0.5 )

  s.add(2)
  assert Close(s.count, 3)
  assert Close(s.mean, 5./3)
  assert Close(s.min, 1)
  assert Close(s.max, 2)
  assert Close(s.stddev, (sum( [ (x - 5./3)**2 for x in [1,2,2] ] ) / 3)**0.5 )

  s.add(0)
  assert Close(s.count, 4)
  assert Close(s.mean, 5./4)
  assert Close(s.min, 0)
  assert Close(s.max, 2)
  assert Close(s.stddev, (sum( [ (x - 5./4)**2 for x in [1,2,2,0] ] ) / 4)**0.5 )

def test_speed():
  ureg = uc.UnitRegistry()

  ureg.addLengthBaseUnit("m")
  ureg.addMassBaseUnit("g")
  ureg.addTimeBaseUnit("s")

  ureg.addUnit("1 in = 2.54 cm")
  ureg.addUnit("1 ft = 12 in")
  ureg.addUnit("1 mile = 5280 ft")
  ureg.addUnit("1 min = 60 s")
  ureg.addUnit("1 hour = 60 min")


  pureg = pint.UnitRegistry()

  q = ureg.makeQuantity(100,'mile/hour')

  pq = pureg.Quantity(100, 'mile/hour')


  bm = Benchmark()

  bm.run( lambda : q.to("km/s") )
  print()
  print("mean:",bm.stats.mean)
  print("count:",bm.stats.count)
  print("err:",bm.stats.error)
  bm.run( lambda : pq.to("km/s") )
  print("mean:",bm.stats.mean)
  print("count:",bm.stats.count)
  print("err:",bm.stats.error)


