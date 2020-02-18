import lib.pyUnitConvert as uc
import pint
from Approx import *
from Benchmark import *




def test_stats():
  s = Stats()

  s.add(1)
  assert s.count == Approx( 1)
  assert s.mean == Approx(  1)
  assert s.min == Approx(  1)
  assert s.max == Approx(  1)
  assert s.stddev == Approx(  0)
  assert s.variance == Approx(  0)
  assert s.error == Approx(  0)

  s.add(2)
  assert s.count == Approx(  2)
  assert s.mean == Approx(  1.5)
  assert s.min == Approx(  1)
  assert s.max == Approx(  2)
  assert s.stddev == Approx(  (2*0.5**2/2)**0.5 )
  assert s.error == Approx(   (2*0.5**2/2)**0.5/2**0.5 )

  s.add(2)
  assert s.count == Approx(  3)
  assert s.mean == Approx(  5./3)
  assert s.min == Approx(  1)
  assert s.max == Approx(  2)
  assert s.stddev == Approx(  (sum( [ (x - 5./3)**2 for x in [1,2,2] ] ) / 3)**0.5 )

  s.add(0)
  assert s.count == Approx(  4)
  assert s.mean == Approx(  5./4)
  assert s.min == Approx(  0)
  assert s.max == Approx(  2)
  assert s.stddev == Approx(  (sum( [ (x - 5./4)**2 for x in [1,2,2,0] ] ) / 4)**0.5 )

def test_speed_simple():
  ureg = uc.UnitRegistry()

  ureg.addUnit("m = [L]")
  ureg.addUnit("g = [M]")
  ureg.addUnit("s = [T]")

  ureg.addUnit("1 in = 2.54 cm")
  ureg.addUnit("1 ft = 12 in")
  ureg.addUnit("1 mile = 5280 ft")
  ureg.addUnit("1 min = 60 s")
  ureg.addUnit("1 hour = 60 min")


  q = ureg.makeQuantity('100 mile/hour')

  pureg = pint.UnitRegistry()
  pq = pureg.Quantity(100, 'mile/hour')


  bm = Benchmark()

  bm.run( lambda : q.to("km/s") )
  uc_runtime = bm.measurement

  bm.run( lambda : pq.to("km/s") )
  pint_runtime = bm.measurement

  print("convert: 100 mph -> km/s")
  print("Pint",pint_runtime)
  print("UnitConvert",uc_runtime)
  print("Speedup",Speedup(pint_runtime,uc_runtime))


def test_speed_medium():
  print()
  ureg = uc.UnitRegistry()

  ureg.addUnit("m = [L]")
  ureg.addUnit("g = [M]")
  ureg.addUnit("s = [T]")

  ureg.addUnit("1 in = 2.54 cm")
  ureg.addUnit("1 ft = 12 in")
  ureg.addUnit("1 mile = 5280 ft")
  ureg.addUnit("1 min = 60 s")
  ureg.addUnit("1 hour = 60 min")
  ureg.addUnit("1 J = kg m^2 / s^2")
  ureg.addUnit("1 W = J / s")


  q = ureg.makeQuantity('100 W')

  pureg = pint.UnitRegistry()
  pq = pureg.Quantity(100, 'W')


  bm = Benchmark()
  bm.run( lambda : pq.to("g cm mm / hour / min / ms") )
  pint_runtime = bm.measurement

  bm.run( lambda : q.to("g cm mm / hour / min / ms") )
  uc_runtime = bm.measurement

  print("convert: 100 W -> g cm mm / hr / min / ms")
  print("Pint",pint_runtime)
  print("UnitConvert",uc_runtime)
  print("Speedup",Speedup(pint_runtime,uc_runtime))



def test_speed_parse_and_convert():
  print()
  ureg = uc.UnitRegistry()

  ureg.addUnit("m = [L]")
  ureg.addUnit("g = [M]")
  ureg.addUnit("s = [T]")

  ureg.addUnit("1 in = 2.54 cm")
  ureg.addUnit("1 ft = 12 in")
  ureg.addUnit("1 mile = 5280 ft")
  ureg.addUnit("1 min = 60 s")
  ureg.addUnit("1 hour = 60 min")
  ureg.addUnit("1 J = kg m^2 / s^2")
  ureg.addUnit("1 W = J / s")


  q = ureg.makeQuantity('100 W')

  pureg = pint.UnitRegistry()
  pq = pureg.Quantity(100, 'W')


  bm = Benchmark()
  bm.run( lambda : pureg.Quantity("100 W").to("g cm mm / hour / min / ms") )
  pint_runtime = bm.measurement

  bm.run( lambda : ureg.makeQuantity("100 W").to("g cm mm / hour / min / ms") )
  uc_runtime = bm.measurement

  print("parse and convert: 100 W -> g cm mm / hr / min / ms")
  print("Pint",pint_runtime)
  print("UnitConvert",uc_runtime)
  print("Speedup",Speedup(pint_runtime,uc_runtime))



