/* Script for -z combreloc: combine and sort reloc sections */

SECTIONS
{
  /********************************
    add my section --> start
  ********************************/
  .register :
  {
    /*bus register*/
    _register_bus_start = .;
    *(.register.bus)
    _register_bus_end = .;
    /* bus exit */
    _unregister_bus_start = .;
    *(.unregister.bus)
    _unregister_bus_end = .;
    /* drvier register */
    _register_driver_start = .;
    *(.register.driver)
    _register_driver_end = .;
  }
  /********************************
    add my section --> end
  ********************************/
}
