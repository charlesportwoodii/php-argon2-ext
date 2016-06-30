--TEST--
Tests Argon2d hash and verify
--FILE--
<?php
$hash = argon2_hash('password', PASSWORD_ARGON2D);
var_dump(argon2_verify('password', $hash));
--EXPECT--
bool(true)