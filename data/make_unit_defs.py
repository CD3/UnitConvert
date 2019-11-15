import pint
import re

ureg = pint.UnitRegistry()
ureg.define("percent = 0.01 rad")

base_units = [u for u in ureg._units if ureg._units[u].is_base]
base_units = [u for u in base_units if u == "{:~}".format(ureg.Quantity(1,u).units)]
other_units = [u for u in ureg._units if not u in base_units]


print("# base unit definitions")
for u in base_units:
  unit_string = "{} = {}".format(u,str(ureg.Quantity(u).dimensionality))
  # replace pint dimension strings with UnitConvert dimension symbols
  unit_string = unit_string.replace("length","L")
  unit_string = unit_string.replace("mass","M")
  unit_string = unit_string.replace("time","T")
  unit_string = unit_string.replace("current","I")
  unit_string = unit_string.replace("substance","N")
  unit_string = unit_string.replace("temperature","THETA")
  unit_string = unit_string.replace("luminosity","J")
  unit_string = unit_string.replace("dimensionless","[1]")
  unit_string = unit_string.replace("printing_unit","1")
  print(unit_string)

for u in other_units:
  name = "{}".format(u)
  definition = "{:~}".format(ureg.Quantity(1,u).to_base_units())
  definition = definition.replace(" ** ","^")

  # remove units that have a number in their name.
  # if re.search("[0-9]",name):
    # continue
  # remove units that have fractional powers
  if re.search("\^1\.5",definition):
    continue

  unit_string = "{} = {}".format(name,definition)
  try:
    # don't print strings with unicode chars
    print(unit_string.encode('ascii').decode('ascii'))
  except:
    pass

