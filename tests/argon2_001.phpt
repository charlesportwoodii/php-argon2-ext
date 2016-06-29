--TEST--
Tests Argon2 constants are defined
--FILE--
<?php
var_dump(PASSWORD_ARGON2_D);
var_dump(PASSWORD_ARGON2_I);
--EXPECT--
int(0)
int(1)