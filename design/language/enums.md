## Enums

`enum speciesTypes { Ki="112",Su=44,La=33.2 }`

`struct animal {`<br>
  &emsp;  `age: int,`   
  &emsp;  `species: speciesTypes,`<br>
  &emsp;  `isMale: bool`<br>
`}`

### properties.

* `.value`
     Provides the underlying value.example `speciesTypes.La` would be a double ie `33.2`

* `.index`
    The count from 0 the enum is at. example`speciesTypes.La` would be at index 2
