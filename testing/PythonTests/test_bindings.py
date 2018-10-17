import UnitConvert as uc
from Approx import *


def test_registry():
  ureg = uc.UnitRegistry()

  ureg.addLengthBaseUnit("m")
  ureg.addMassBaseUnit("g")
  ureg.addTimeBaseUnit("s")

  ureg.addUnit("1 in = 2.54 cm")
  ureg.addUnit("1 ft = 12 in")
  ureg.addUnit("1 mile = 5280 ft")
  ureg.addUnit("1 min = 60 s")
  ureg.addUnit("1 hour = 60 min")

  q = ureg.makeQuantity( 1, 'm')
  q2 = q.to("cm")

  assert q2.value() == Approx(100)

