--TEST--
Tests Argon2d hash and verify
--FILE--
<?php
$hash = argon2_hash('password', ARGON2D_PASSWORD);
var_dump(argon2_verify('password', $hash));
var_dump(argon2_verify('badpass', $hash));
--EXPECT--
bool(true)
bool(false)