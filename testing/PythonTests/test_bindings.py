try:
  import lib.pyUnitConvert as uc
except:
  import pyUnitConvert as uc

from Approx import *


def test_registry():
  ureg = uc.UnitRegistry()

  ureg.addUnit("m = [L]")
  ureg.addUnit("g = [M]")
  ureg.addUnit("s = [T]")

  ureg.addUnit("1 in = 2.54 cm")
  ureg.addUnit("1 ft = 12 in")
  ureg.addUnit("1 mile = 5280 ft")
  ureg.addUnit("1 min = 60 s")
  ureg.addUnit("1 hour = 60 min")
  ureg.addUnit("1 mph = 1 mile / hour")

  q = ureg.makeQuantity('1 m')
  print(type(q))
  q2 = q.to("cm")
  print(type(q2))

  assert q2.value() == Approx(100)


  q = ureg.makeQuantity('100 mph')
  q2 = q.to("m/s")

  assert q2.value() == Approx(44.704)
