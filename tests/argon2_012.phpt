--TEST--
Tests Argon2 namespace support
--FILE--
<?php
$hash = \Argon2\Hash('password');
var_dump(\Argon2\Verify('password', $hash));
var_dump(\Argon2\Info($hash));
--EXPECT--
bool(true)
array(2) {
  ["algorithm"]=>
  string(8) "argon2id"
  ["options"]=>
  array(3) {
    ["m_cost"]=>
    int(65536)
    ["t_cost"]=>
    int(3)
    ["threads"]=>
    int(1)
  }
}