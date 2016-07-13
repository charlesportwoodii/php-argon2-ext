--TEST--
Tests Argon2 constants are defined
--FILE--
<?php
var_dump(ARGON2_PASSWORD);
var_dump(ARGON2I_PASSWORD);
var_dump(ARGON2D_PASSWORD);
--EXPECT--
int(1)
int(1)
int(0)