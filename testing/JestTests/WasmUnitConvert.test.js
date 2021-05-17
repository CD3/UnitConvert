var UnitConvert = require("./WasmUnitConvert.js");

test('can convert quantity string from meters to feet', done => {

  UnitConvert().then(
    instance => {
      var length = instance.UnitConvertString("2 m","ft");
      expect( length ).toMatch(/^6.56.*ft$/);
      done();
    }
  )
});

test('can meters to centimeter and get magnitude', done => {

  UnitConvert().then(
    instance => {
      var length = instance.GetMagnitudeInUnit("2 m","cm");
      expect( length ).toBeCloseTo(200);
      done();
    }
  )
});


test('check if two quantities have same dimension', done => {

  UnitConvert().then(
    instance => {
      expect( instance.HaveSameDimensions("2 m", "ft") ).toBeTruthy();
      expect( instance.HaveSameDimensions("2 m", "s") ).toBeFalsy();
      done();
    }
  )
});

test('can add unit definition to registry', done => {

  UnitConvert().then(
    instance => {
      instance.AddUnitDefinition("football_field = 100 yd");
      var length = instance.GetMagnitudeInUnit("150 ft","football_field");
      expect( length ).toBeCloseTo(0.5);

      done();
    }
  )
});
