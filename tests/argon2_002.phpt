--TEST--
Tests Argon2 hash and verify without options
--FILE--
<?php
$hash = argon2_hash('password');
var_dump(argon2_verify('password', $hash));
--EXPECT--
bool(true)