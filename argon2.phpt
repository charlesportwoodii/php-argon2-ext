<?php

var_dump(PASSWORD_ARGON2_D);
var_dump(PASSWORD_ARGON2_I);

var_dump(argon2_hash('test'));

var_dump(argon2_hash('test', PASSWORD_ARGON2_D));
var_dump(argon2_hash('test', PASSWORD_ARGON2_I));

var_dump(argon2_hash('test', PASSWORD_ARGON2_D, [ 'm_cost' => 12, 't_cost' => 3] ));
var_dump(argon2_hash('test', PASSWORD_ARGON2_I, [ 'm_cost' => 12, 't_cost' => 3] ));