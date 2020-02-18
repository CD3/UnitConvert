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
  ureg = uc.getGlobalUnitRegistry()
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
  ureg = uc.getGlobalUnitRegistry()
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

  ureg = uc.getGlobalUnitRegistry()
  q = ureg.makeQuantity('100 W')

  pureg = pint.UnitRegistry()
  pq = pureg.Quantity(100, 'W')







  bm = Benchmark()
  bm.run( lambda : pureg.Quantity("100 m").to("cm") )
  pint_runtime = bm.measurement

  bm.run( lambda : ureg.makeQuantity("100 m").to("cm") )
  uc_runtime = bm.measurement

  print("parse and convert: 100 m -> cm")
  print("Pint",pint_runtime)
  print("UnitConvert",uc_runtime)
  print("Speedup",Speedup(pint_runtime,uc_runtime))


  print()
  print()






  bm = Benchmark()
  bm.run( lambda : pureg.Quantity("100 W").to("g cm mm / hour / min / ms") )
  pint_runtime = bm.measurement

  bm.run( lambda : ureg.makeQuantity("100 W").to("g cm mm / hour / min / ms") )
  uc_runtime = bm.measurement

  print("parse and convert: 100 W -> g cm mm / hr / min / ms")
  print("Pint",pint_runtime)
  print("UnitConvert",uc_runtime)
  print("Speedup",Speedup(pint_runtime,uc_runtime))



def test_pint_calc_with_uc_conversion():
  print()
  ureg = uc.getGlobalUnitRegistry()
  pureg = pint.UnitRegistry()

  l = pureg.Quantity(100,'in')
  w = pureg.Quantity(100,'cm')

  A = l*w

  bm = Benchmark()
  bm.run( lambda : A.to("acre") )
  pint_runtime = bm.measurement

  bm.run( lambda : pureg.Quantity(ureg.makeQuantity(str(A)).to("acre").value(),"acre" ))
  uc_runtime = bm.measurement

  print("pint calc with uc conversion")
  print("Pint",pint_runtime)
  print("UnitConvert",uc_runtime)
  print("Speedup",Speedup(pint_runtime,uc_runtime))

