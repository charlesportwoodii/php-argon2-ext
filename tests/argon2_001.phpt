--TEST--
Tests Argon2 constants are defined
--FILE--
<?php
var_dump(HASH_ARGON2);
var_dump(HASH_ARGON2I);
var_dump(HASH_ARGON2D);
--EXPECT--
int(1)
int(1)
int(0)