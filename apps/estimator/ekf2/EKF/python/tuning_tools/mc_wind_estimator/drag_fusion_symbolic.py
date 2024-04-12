#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

File: frag_fusion_symbolic.py
Author: Mathieu Bresciani <mathieu@auterion.com>
License: BSD 3-Clause
Description:
"""

from sympy import *

V = Symbol("V", real=True)
rho = Symbol("rho", real=True)
rho_n = Symbol("rho_n", real=True)
Mcoef = Symbol("Mcoef", real=True)
Bcoef = Symbol("Bcoef", real=True)
a = Symbol("a", real=True)

f1 = 0.5 * rho / Bcoef * V**2 + rho_n * Mcoef * V - a

print("If Bcoef > 0 and Mcoef > 0")
print("V =")
res_V = solve(f1, V)
res_V = res_V[0]
pprint(res_V)
print("a_pred =")
pprint(solve(f1, a))
