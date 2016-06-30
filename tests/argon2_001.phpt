--TEST--
Tests Argon2 constants are defined
--FILE--
<?php
var_dump(PASSWORD_ARGON2D);
var_dump(PASSWORD_ARGON2I);
--EXPECT--
int(0)
int(1)