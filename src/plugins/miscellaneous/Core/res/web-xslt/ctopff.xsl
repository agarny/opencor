<x:stylesheet
  version="2.0"
  xmlns:x="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1998/Math/MathML"
  xmlns:m="http://www.w3.org/1998/Math/MathML"
  exclude-result-prefixes="m"
>

<!--

Copyright David Carlisle 2001, 2002, 2008, 2009, 2013.
FF version with modified namespace and mode usage

Use and distribution of this code are permitted under the terms of the <a
href="http://www.w3.org/Consortium/Legal/copyright-software-19980720"
>W3C Software Notice and License</a>.
Or the Apache 2, MIT or MPL 1.1 or MPL 2.0 licences.
2001-2002 MathML2 version
2008-2009     Updates for MathML3
-->

<x:output method="xml" />

<x:template match="/">
  <x:apply-templates/>
</x:template>

<x:template match="*">
<x:copy>
  <x:copy-of select="@*"/>
  <x:apply-templates/>
</x:copy>
</x:template>


<!-- 4.4.1.1 cn -->

<x:template match="m:cn">
 <mn><x:apply-templates/></mn>
</x:template>

<x:template match="m:cn[@type='complex-cartesian']">
  <mrow>
    <mn><x:apply-templates select="text()[1]"/></mn>
    <mo>+</mo>
    <mn><x:apply-templates select="text()[2]"/></mn>
    <mo>&#8290;<!--invisible times--></mo>
    <mi>i<!-- imaginary i --></mi>
  </mrow>
</x:template>

<x:template match="m:apply[*[1][self::m:csymbol='complex_cartesian']]">
  <mrow>
    <mn><x:apply-templates select="*[2]"/></mn>
    <mo>+</mo>
    <mn><x:apply-templates select="*[3]"/></mn>
    <mo>&#8290;<!--invisible times--></mo>
    <mi>i<!-- imaginary i --></mi>
  </mrow>
</x:template>

<x:template match="m:cn[@type='rational']">
  <mrow>
    <mn><x:apply-templates select="text()[1]"/></mn>
    <mo>/</mo>
    <mn><x:apply-templates select="text()[2]"/></mn>
  </mrow>
</x:template>

<x:template match="m:apply[*[1][self::m:csymbol='rational']]">
  <mrow>
    <mn><x:apply-templates select="*[2]"/></mn>
    <mo>/</mo>
    <mn><x:apply-templates select="*[3]"/></mn>
  </mrow>
</x:template>

<x:template match="m:cn[not(@type) or @type='integer']">
  <x:choose>
  <x:when test="not(@base) or @base=10">
       <mn><x:apply-templates/></mn>
  </x:when>
  <x:otherwise>
  <msub>
    <mn><x:apply-templates/></mn>
    <mn><x:value-of select="@base"/></mn>
  </msub>
  </x:otherwise>
  </x:choose>
</x:template>

<x:template match="m:cn[@type='complex-polar']">
  <mrow>
    <mn><x:apply-templates select="text()[1]"/></mn>
    <mo>&#8290;<!--invisible times--></mo>
    <msup>
    <mi>e<!-- exponential e--></mi>
    <mrow>
     <mi>i<!-- imaginary i--></mi>
     <mo>&#8290;<!--invisible times--></mo>
     <mn><x:apply-templates select="text()[2]"/></mn>
    </mrow>
    </msup>
  </mrow>
</x:template>

<x:template match="m:apply[*[1][self::m:csymbol='complex_polar']]">
  <mrow>
    <x:apply-templates select="*[2]"/>
    <mo>&#8290;<!--invisible times--></mo>
    <msup>
    <mi>e<!-- exponential e--></mi>
    <mrow>
     <mi>i<!-- imaginary i--></mi>
     <mo>&#8290;<!--invisible times--></mo>
     <x:apply-templates select="*[3]"/>
    </mrow>
    </msup>
  </mrow>
</x:template>

<x:template match="m:cn[@type='e-notation']">
  <mrow>
    <mn>
      <x:apply-templates select="m:sep/preceding-sibling::node()"/>
    </mn>
    <mo>&#183;<!-- middle dot --></mo>
    <msup>
      <mn>10</mn>
      <mn>
        <x:apply-templates select="m:sep/following-sibling::node()"/>
      </mn>
    </msup>
  </mrow>
</x:template>

<x:template match="m:cn[@type='hexdouble']">
  <mn>
    <x:text>0x</x:text>
    <x:apply-templates/>
  </mn>
</x:template>

<!-- 4.4.1.1 ci  -->

<x:template match="m:ci/text()">
 <mi mathvariant="italic"><x:value-of select="."/></mi>
</x:template>

<x:template match="m:ci">
 <x:apply-templates/>
</x:template>

<!-- 4.4.1.2 csymbol -->

<x:template match="m:csymbol/text()">
 <mi><x:value-of select="."/></mi><!-- Robin Green r.d.greenATlancaster.ac.uk, Christoph Lange langecATweb.de-->
</x:template>

<x:template match="m:csymbol">
 <x:apply-templates/>
</x:template>

<!-- 4.4.2.1 apply 4.4.2.2 reln -->

<x:template match="m:apply|m:reln">
 <mrow>
   <x:choose>
     <x:when test="*[1]/*/*">
       <mfenced separators="">
         <x:apply-templates select="*[1]">
           <x:with-param name="p" select="10"/>
         </x:apply-templates>
       </mfenced>
     </x:when>
     <x:otherwise>
       <x:apply-templates select="*[1]">
         <x:with-param name="p" select="10"/>
       </x:apply-templates>
     </x:otherwise>
   </x:choose>
 <mo>&#8289;<!--function application--></mo>
 <mfenced open="(" close=")" separators=",">
 <x:apply-templates select="*[position()>1]"/>
 </mfenced>
 </mrow>
</x:template>


<x:template match="m:bind">
 <mrow>
   <x:choose>
     <x:when test="*[1]/*/*">
       <mfenced separators="">
         <x:apply-templates select="*[1]">
           <x:with-param name="p" select="10"/>
         </x:apply-templates>
       </mfenced>
     </x:when>
     <x:otherwise>
       <x:apply-templates select="*[1]">
         <x:with-param name="p" select="10"/>
       </x:apply-templates>
     </x:otherwise>
   </x:choose>
   <x:apply-templates select="bvar/*"/>
   <mo>.</mo>
   <x:apply-templates select="*[position()>1][not(self::m:bvar)]"/>
 </mrow>
</x:template>

<!-- 4.4.2.3 fn -->
<x:template match="m:fn">
 <mrow><x:apply-templates/></mrow>
</x:template>

<!-- 4.4.2.4 interval -->
<x:template match="m:interval[*[2]]">
 <mfenced open="[" close="]"><x:apply-templates/></mfenced>
</x:template>
<x:template match="m:interval[*[2]][@closure='open']" priority="2">
 <mfenced open="(" close=")"><x:apply-templates/></mfenced>
</x:template>
<x:template match="m:interval[*[2]][@closure='open-closed']" priority="2">
 <mfenced open="(" close="]"><x:apply-templates/></mfenced>
</x:template>
<x:template match="m:interval[*[2]][@closure='closed-open']" priority="2">
 <mfenced open="[" close=")"><x:apply-templates/></mfenced>
</x:template>

<x:template match="m:interval">
 <mfenced open="{{" close="}}"><x:apply-templates/></mfenced>
</x:template>

<x:template match="m:apply[*[1][self::m:csymbol='integer_interval']]">
 <mfenced open="[" close="]"><x:apply-templates select="*[position()!=1]"/></mfenced>
</x:template>
<x:template match="m:apply[*[1][self::m:csymbol='interval']]">
 <mfenced open="[" close="]"><x:apply-templates select="*[position()!=1]"/></mfenced>
</x:template>
<x:template match="m:apply[*[1][self::m:csymbol='interval-cc']]">
 <mfenced open="[" close="]"><x:apply-templates select="*[position()!=1]"/></mfenced>
</x:template>
<x:template match="m:apply[*[1][self::m:csymbol='interval-oo']]">
 <mfenced open="(" close=")"><x:apply-templates select="*[position()!=1]"/></mfenced>
</x:template>
<x:template match="m:apply[*[1][self::m:csymbol='oriented_interval']]">
 <mfenced open="(" close=")"><x:apply-templates select="*[position()!=1]"/></mfenced>
</x:template>

<!-- 4.4.2.5 inverse -->

<x:template match="m:apply[*[1][self::m:inverse]]
                       |m:apply[*[1][self::m:csymbol='inverse']]">
 <msup>
  <x:apply-templates select="*[2]"/>
  <mrow><mo>(</mo><mn>-1</mn><mo>)</mo></mrow>
 </msup>
</x:template>

<!-- 4.4.2.6 sep -->

<!-- 4.4.2.7 condition -->
<x:template match="m:condition">
 <mrow><x:apply-templates/></mrow>
</x:template>

<!-- 4.4.2.8 declare -->
<x:template match="m:declare"/>

<!-- 4.4.2.9 lambda -->
<x:template match="m:lambda
                                |m:apply[*[1][self::m:csymbol='lambda']]
                                |m:bind[*[1][self::m:csymbol='lambda']]"><!--dpc-->
 <mrow>
  <mi>&#955;<!--lambda--></mi>
  <mrow>
   <x:choose>
    <x:when test="m:condition">
     <x:apply-templates select="m:condition/*"/>
    </x:when>
    <x:otherwise>
     <x:apply-templates select="m:bvar/*"/>
    </x:otherwise>
   </x:choose>
   <x:if test="m:domainofapplication">
    <mo>&#x2208;</mo>
    <x:apply-templates select="m:domainofapplication/*"/>
   </x:if>
  </mrow>
  <mo>.</mo>
  <mfenced>
   <x:apply-templates select="*[last()]"/>
  </mfenced>
 </mrow>
</x:template>

<x:template match="m:lambda[not(m:bvar)]" priority="2">
 <mrow>
  <x:apply-templates select="*[last()]"/>
  <msub>
   <mo>|</mo>
   <mrow>
    <x:apply-templates select="m:condition|m:interval|m:domainofapplication/*"/>
   </mrow>
  </msub>
 </mrow>
</x:template>


<!-- 4.4.2.10 compose -->
<x:template match="m:apply[*[1][self::m:compose]]
                       |m:apply[*[1][self::m:csymbol='left_compose']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="1"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&#8728;<!-- o --></mo></x:with-param>
</x:call-template>
</x:template>


<!-- 4.4.2.11` ident -->
<x:template match="m:ident">
<mi>id</mi>
</x:template>

<!-- 4.4.2.12` domain -->
<x:template match="m:domain">
<mi>domain</mi>
</x:template>

<!-- 4.4.2.13` codomain -->
<x:template match="m:codomain">
<mi>codomain</mi>
</x:template>

<!-- 4.4.2.14` image -->
<x:template match="m:image">
<mi>image</mi>
</x:template>

<!-- 4.4.2.15` domainofapplication -->
<x:template match="m:domainofapplication">
 <merror><mtext>unexpected domainofapplication</mtext></merror>
</x:template>

<x:template match="m:apply[*[2][self::m:bvar]][m:domainofapplication]" priority="0.4">
 <mrow>
  <munder>
   <x:apply-templates select="*[1]"/>
   <mrow>
    <x:apply-templates select="m:bvar/*"/>
    <mo>&#8712;<!-- in --></mo>
    <x:apply-templates select="m:domainofapplication/*"/>
   </mrow>
  </munder>
  <mfenced>
   <x:apply-templates select="m:domainofapplication/following-sibling::*"/>
  </mfenced>
 </mrow>
</x:template>

<x:template match="m:apply[m:domainofapplication]" priority="0.3">
 <mrow>
  <mrow><mi>restriction</mi>
  <mfenced>
   <x:apply-templates select="*[1]"/>
   <x:apply-templates select="m:domainofapplication/*"/>
  </mfenced>
  </mrow>
  <mfenced>
   <x:apply-templates select="m:domainofapplication/following-sibling::*"/>
  </mfenced>
 </mrow>
</x:template>

<!-- 4.4.2.16` piecewise -->
<x:template match="m:piecewise">
  <mrow>
    <mo>{</mo>
    <mtable>
      <x:for-each select="m:piece|m:otherwise">
        <mtr>
          <mtd><x:apply-templates select="*[1]"/></mtd>
          <x:choose><!--dpc-->
            <x:when  test="self::m:piece">
              <mtd columnalign="left"><mtext>&#160; if &#160;</mtext></mtd>
              <mtd><x:apply-templates select="*[2]"/></mtd>
            </x:when>
            <x:otherwise>
              <mtd columnspan="2" columnalign="left"><mtext>&#160; otherwise</mtext></mtd>
            </x:otherwise>
          </x:choose>
        </mtr>
      </x:for-each>
    </mtable>
  </mrow>
</x:template>


<!-- 4.4.3.1 quotient -->
<x:template match="m:apply[*[1][self::m:quotient]]
                       |m:apply[*[1][self::m:csymbol='quotient']]">
<mrow>
<mo>&#8970;<!-- lfloor--></mo>
<x:apply-templates select="*[2]"/>
<mo>/</mo>
<x:apply-templates select="*[3]"/>
<mo>&#8971;<!-- rfloor--></mo>
</mrow>
</x:template>



<!-- 4.4.3.2 factorial -->
<x:template match="m:apply[*[1][self::m:factorial]]
                                |m:apply[*[1][self::m:csymbol='factorial']]">
<mrow>
<x:apply-templates select="*[2]">
  <x:with-param name="p" select="7"/>
</x:apply-templates>
<mo>!</mo>
</mrow>
</x:template>


<!-- 4.4.3.3 divide -->
<x:template match="m:apply[*[1][self::m:divide]]
                                |m:apply[*[1][self::m:csymbol='divide']]">
  <x:param name="p" select="0"/>
<x:choose>
 <x:when test="$p &gt;= 5 and name(../*[1]) = 'power'">
  <mrow>
    <mo>(</mo>
    <mfrac>
      <x:apply-templates select="*[position()!=1]"/>
    </mfrac>
    <mo>)</mo>
  </mrow>
 </x:when>
 <x:otherwise>
  <mfrac>
    <x:apply-templates select="*[position()!=1]"/>
  </mfrac>
 </x:otherwise>
</x:choose>
</x:template>


<!-- 4.4.3.4 max  min-->
<x:template match="m:apply[*[1][self::m:max]]
                                |m:apply[*[1][self::m:csymbol='max']]">
<mrow>
  <mi>max</mi>
  <x:call-template name="set">
    <x:with-param name="o" select="'('"/>
    <x:with-param name="c" select="')'"/>
  </x:call-template>
</mrow>
</x:template>

<x:template match="m:apply[*[1][self::m:min]]
                                |m:apply[*[1][self::m:csymbol='min']]">
<mrow>
  <mi>min</mi><!--dpc-->
  <x:call-template name="set">
    <x:with-param name="o" select="'('"/>
    <x:with-param name="c" select="')'"/>
  </x:call-template>
</mrow>
</x:template>

<!-- 4.4.3.5  minus-->
<x:template match="m:apply[*[1][self::m:minus] and count(*)=2]
                                |m:apply[*[1][self::m:csymbol='unary_minus']]">
  <x:param name="p" select="0"/>
<mrow>
  <x:if test="$p &gt;= 5"><mo>(</mo></x:if>
  <mo>&#8722;<!--minus--></mo>
  <x:apply-templates select="*[2]">
      <x:with-param name="p" select="5"/>
  </x:apply-templates>
  <x:if test="$p &gt;= 5"><mo>)</mo></x:if>
</mrow>
</x:template>

<x:template match="m:apply[*[1][self::m:minus] and count(*)&gt;2]
                                |m:apply[*[1][self::m:csymbol='minus']]">
  <x:param name="p" select="0"/>
<x:call-template name="binary">
  <x:with-param name="mo"><mo>&#8722;<!--minus--></mo></x:with-param>
  <x:with-param name="p" select="$p"/>
  <x:with-param name="this-p" select="2"/>
</x:call-template>
</x:template>

<!-- 4.4.3.6  plus-->
<!-- put out - on leading term (mathjax report)-->
<x:template match="m:apply[*[1][self::m:plus]]
                                |m:apply[*[1][self::m:csymbol='plus']]">
  <x:param name="p" select="0"/>
  <x:param name="op" select="name(../*[1])"/>
  <mrow>
  <x:if test="($p &gt; 2 and $op != 'minus') or ($op = 'minus' and (. != ../*[2] or count(../*) = 2) and count(*) != 2)"><mo>(</mo></x:if>
  <x:for-each select="*[position()&gt;1]">
   <x:choose>
    <x:when test="self::m:apply[count(*)=2 and *[1][self::m:minus]] or
            self::m:cn[not(m:sep) and (number(.) &lt; 0)]">
     <mo>&#8722;<!--minus--></mo>
    </x:when>
    <x:when test="position()!=1">
     <mo>+</mo>
    </x:when>
   </x:choose>
   <x:choose>
    <x:when test="self::m:cn[not(m:sep) and (number(.) &lt; 0)]">
     <mn><x:value-of select="-(.)"/></mn>
    </x:when>
    <x:when test=" self::m:apply[count(*)=2 and *[1][self::m:minus]]">
     <x:apply-templates select="*[2]">
      <x:with-param name="first" select="1"/>
      <x:with-param name="p" select="2"/>
     </x:apply-templates>
    </x:when>
    <x:when test="self::m:apply[*[1][self::m:times] and
            *[2][self::m:cn[not(m:sep) and (number(.) &lt;0)]]]">
     <mrow>
      <mn><x:value-of select="-(*[2])"/></mn>
      <mo>&#8290;<!--invisible times--></mo>
      <x:apply-templates select=".">
       <x:with-param name="first" select="2"/>
       <x:with-param name="p" select="2"/>
      </x:apply-templates>
     </mrow>
    </x:when>
    <x:when test="self::m:apply[*[1][self::m:times] and count(*[2][self::m:apply]/*)>3 and
            *[2][self::m:apply/*[1][self::m:minus]]]">
     <mrow>
      <x:apply-templates select="./*[2]/*[2]"/>
      <x:apply-templates select=".">
       <x:with-param name="first" select="2"/>
       <x:with-param name="p" select="2"/>
      </x:apply-templates>
     </mrow>
    </x:when>
    <x:otherwise>
     <x:apply-templates select=".">
      <x:with-param name="p" select="2"/>
     </x:apply-templates>
    </x:otherwise>
   </x:choose>
  </x:for-each>
  <x:if test="($p &gt; 2 and $op != 'minus') or ($op = 'minus' and (. != ../*[2] or count(../*) = 2) and count(*) != 2)"><mo>)</mo></x:if>
  </mrow>
</x:template>


<!-- 4.4.3.7 power -->
<x:template match="m:apply[*[1][self::m:power]]
                                |m:apply[*[1][self::m:csymbol='power']]">
<x:choose>
 <x:when test="name(../*[1]) = 'power' and name(*[3]) = 'apply'">
  <mrow>
    <mo>(</mo>
    <msup>
      <x:apply-templates select="*[2]">
        <x:with-param name="p" select="5"/>
      </x:apply-templates>
      <x:apply-templates select="*[3]"/>
    </msup>
    <mo>)</mo>
  </mrow>
 </x:when>
 <x:otherwise>
   <msup>
     <x:apply-templates select="*[2]">
       <x:with-param name="p" select="5"/>
     </x:apply-templates>
     <x:apply-templates select="*[3]"/>
   </msup>
 </x:otherwise>
</x:choose>
</x:template>

<!-- 4.4.3.8 remainder -->
<x:template match="m:apply[*[1][self::m:rem]]
                       |m:apply[*[1][self::m:csymbol='rem']]">
<mrow>
  <mi>rem</mi>
  <x:call-template name="set">
    <x:with-param name="o" select="'('"/>
    <x:with-param name="c" select="')'"/>
  </x:call-template>
</mrow>
</x:template>

<!-- 4.4.3.9  times-->
<x:template match="m:apply[*[1][self::m:times]]
                                |m:apply[*[1][self::m:csymbol='times']]"
              name="times">
  <x:param name="p" select="0"/>
  <x:param name="first" select="1"/>
  <x:param name="op" select="name(../*[1])"/>
  <mrow>
  <x:if test="$p &gt; 3 and $op != 'minus'"><mo>(</mo></x:if>
  <x:for-each select="*[position()&gt;1]">
   <x:if test="position() &gt; 1">
    <mo>&#183;<!-- middle dot --></mo>
   </x:if>
   <x:if test="position()&gt;= $first">
   <x:apply-templates select=".">
     <x:with-param name="p" select="3"/>
   </x:apply-templates>
   </x:if>
  </x:for-each>
  <x:if test="$p &gt; 3 and $op != 'minus'"><mo>)</mo></x:if>
  </mrow>
</x:template>


<!-- 4.4.3.10 root -->
<x:template match="m:apply[*[1][self::m:root] and not(m:degree) or number(m:degree)=2]" priority="4">
<msqrt>
<x:apply-templates select="*[position()&gt;1]"/>
</msqrt>
</x:template>

<x:template match="m:apply[*[1][self::m:root]]">
<mroot>
<x:apply-templates select="*[position()&gt;1 and not(self::m:degree)]"/>
<mrow><x:apply-templates select="m:degree/*"/></mrow>
</mroot>
</x:template>


<x:template match="m:apply[*[1][self::m:csymbol='root']]">
<mroot>
  <x:apply-templates select="*[position()!=1]"/>
</mroot>
</x:template>

<!-- 4.4.3.11 gcd -->
<x:template match="m:apply[*[1][self::m:gcd]]
                                |m:apply[*[1][self::m:csymbol='gcd']]">
<mrow>
  <mi>gcd</mi>
  <x:call-template name="set">
    <x:with-param name="o" select="'('"/>
    <x:with-param name="c" select="')'"/>
  </x:call-template>
</mrow>
</x:template>

<!-- 4.4.3.12 and -->
<x:template match="m:apply[*[1][self::m:and]]
                                |m:reln[*[1][self::m:and]]
                                |m:apply[*[1][self::m:csymbol='and']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="2"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>and</mo></x:with-param>
</x:call-template>
</x:template>


<!-- 4.4.3.13 or -->
<x:template match="m:apply[*[1][self::m:or]]
                       |m:apply[*[1][self::m:csymbol='or']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="3"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>or</mo></x:with-param>
</x:call-template>
</x:template>

<!-- 4.4.3.14 xor -->
<x:template match="m:apply[*[1][self::m:xor]]
                       |m:apply[*[1][self::m:csymbol='xor']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="3"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>xor</mo></x:with-param>
</x:call-template>
</x:template>


<!-- 4.4.3.15 not -->
<x:template match="m:apply[*[1][self::m:not]]
                       |m:apply[*[1][self::m:csymbol='not']]">
<mrow>
<mo>not</mo>
<x:apply-templates select="*[2]">
  <x:with-param name="p" select="7"/>
</x:apply-templates>
</mrow>
</x:template>




<!-- 4.4.3.16 implies -->
<x:template match="m:apply[*[1][self::m:implies]]
                                |m:reln[*[1][self::m:implies]]
                                |m:apply[*[1][self::m:csymbol='implies']]">
  <x:param name="p" select="0"/>
<x:call-template name="binary">
  <x:with-param name="mo"><mo>&#8658;<!-- Rightarrow --></mo></x:with-param>
  <x:with-param name="p" select="$p"/>
  <x:with-param name="this-p" select="3"/>
</x:call-template>
</x:template>


<!-- 4.4.3.17 forall -->
<x:template match="m:apply[*[1][self::m:forall]]
                       |m:apply[*[1][self::m:csymbol='forall']]
                       |m:bind[*[1][self::m:forall]]
                       |m:bind[*[1][self::m:csymbol='forall']]">
 <mrow>
  <mo>&#8704;<!--forall--></mo>
 <mrow><x:apply-templates select="m:bvar[not(current()/m:condition)]/*|m:condition/*"/></mrow>
 <mo>.</mo>
 <mfenced>
  <x:apply-templates select="*[last()]"/>
 </mfenced>
</mrow>
</x:template>



<!-- 4.4.3.18 exists -->
<x:template match="m:apply[*[1][self::m:exists]]
                       |m:apply[*[1][self::m:csymbol='exists']]
                       |m:bind[*[1][self::m:exists]]
                       |m:bind[*[1][self::m:csymbol='exists']]">
 <mrow>
  <mo>&#8707;<!--exists--></mo>
 <mrow><x:apply-templates select="m:bvar[not(current()/m:condition)]/*|m:condition/*"/></mrow>
 <mo>.</mo>
 <mfenced separators="">
   <x:choose>
     <x:when test="m:condition">
       <x:apply-templates select="m:condition/*"/>
       <mo>&#8743;<!-- and --></mo>
     </x:when>
     <x:when test="m:domainofapplication">
       <mrow>
       <mrow>
         <x:for-each select="m:bvar">
           <x:apply-templates/>
           <x:if test="position()!=last()">
             <mo>,</mo>
           </x:if>
         </x:for-each>
       </mrow>
       <mo>&#8712;<!-- in --></mo>
       <x:apply-templates select="m:domainofapplication/*"/>
       </mrow>
       <mo>&#8743;<!-- and --></mo>
     </x:when>
   </x:choose>
  <x:apply-templates select="*[last()]"/>
 </mfenced>
</mrow>
</x:template>



<!-- 4.4.3.19 abs -->
<x:template match="m:apply[*[1][self::m:abs]]
                       |m:apply[*[1][self::m:csymbol='abs']]">
<mrow>
<mo>|</mo>
<x:apply-templates select="*[2]"/>
<mo>|</mo>
</mrow>
</x:template>



<!-- 4.4.3.20 conjugate -->
<x:template match="m:apply[*[1][self::m:conjugate]]
                       |m:apply[*[1][self::m:csymbol='conjugate']]">
<mover>
<x:apply-templates select="*[2]"/>
<mo>&#175;<!-- overline --></mo>
</mover>
</x:template>

<!-- 4.4.3.21 arg -->
<x:template match="m:arg">
 <mi>arg</mi>
</x:template>


<!-- 4.4.3.22 real -->
<x:template match="m:real|m:csymbol[.='real']">
 <mo>&#8475;<!-- real --></mo>
</x:template>

<!-- 4.4.3.23 imaginary -->
<x:template match="m:imaginary|m:csymbol[.='imaginary']">
 <mo>&#8465;<!-- imaginary --></mo>
</x:template>

<!-- 4.4.3.24 lcm -->
<x:template match="m:apply[*[1][self::m:lcm]]
                                |m:apply[*[1][self::m:csymbol='lcm']]">
<mrow>
  <mi>lcm</mi>
  <x:call-template name="set">
    <x:with-param name="o" select="'('"/>
    <x:with-param name="c" select="')'"/>
  </x:call-template>
</mrow>
</x:template>


<!-- 4.4.3.25 floor -->
<x:template match="m:apply[*[1][self::m:floor]]
                       |m:apply[*[1][self::m:csymbol='floor']]">
<mrow>
<mo>&#8970;<!-- lfloor--></mo>
<x:apply-templates select="*[2]"/>
<mo>&#8971;<!-- rfloor--></mo>
</mrow>
</x:template>


<!-- 4.4.3.25 ceiling -->
<x:template match="m:apply[*[1][self::m:ceiling]]
                       |m:apply[*[1][self::m:csymbol='ceiling']]">
<mrow>
<mo>&#8968;<!-- lceil--></mo>
<x:apply-templates select="*[2]"/>
<mo>&#8969;<!-- rceil--></mo>
</mrow>
</x:template>

<!-- 4.4.4.1 eq -->
<x:template match="m:apply[*[1][self::m:eq]]
                                |m:reln[*[1][self::m:eq]]
                                |m:apply[*[1][self::m:csymbol='eq']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="1"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>=</mo></x:with-param>
</x:call-template>
</x:template>

<!-- 4.4.4.2 neq -->
<x:template match="m:apply[*[1][self::m:neq]]
                       |m:apply[*[1][self::m:csymbol='neq']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="1"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&#8800;<!-- neq --></mo></x:with-param>
</x:call-template>
</x:template>

<!-- 4.4.4.3 gt -->
<x:template match="m:apply[*[1][self::m:gt]]
                                |m:reln[*[1][self::m:gt]]
                                |m:apply[*[1][self::m:csymbol='gt']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="1"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&gt;</mo></x:with-param>
</x:call-template>
</x:template>

<!-- 4.4.4.4 lt -->
<x:template match="m:apply[*[1][self::m:lt]]
                                |m:reln[*[1][self::m:lt]]
                                |m:apply[*[1][self::m:csymbol='lt']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="1"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&lt;</mo></x:with-param>
</x:call-template>
</x:template>

<!-- 4.4.4.5 geq -->
<x:template match="m:apply[*[1][self::m:geq]]
                                |m:apply[*[1][self::m:csymbol='geq']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="1"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&#8805;</mo></x:with-param>
</x:call-template>
</x:template>

<!-- 4.4.4.6 leq -->
<x:template match="m:apply[*[1][self::m:leq]]
                       |m:apply[*[1][self::m:csymbol='leq']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="1"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&#8804;</mo></x:with-param>
</x:call-template>
</x:template>

<!-- 4.4.4.7 equivalent -->
<x:template match="m:apply[*[1][self::m:equivalent]]
                       |m:apply[*[1][self::m:csymbol='equivalent']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="1"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&#8801;</mo></x:with-param>
</x:call-template>
</x:template>

<!-- 4.4.4.8 approx -->
<x:template match="m:apply[*[1][self::m:approx]]
                       |m:apply[*[1][self::m:csymbol='approx']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="1"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&#8771;</mo></x:with-param>
</x:call-template>
</x:template>


<!-- 4.4.4.9 factorof -->
<x:template match="m:apply[*[1][self::m:factorof]]
                       |m:apply[*[1][self::m:csymbol='factorof']]">
  <x:param name="p" select="0"/>
<x:call-template name="binary">
  <x:with-param name="mo"><mo>|</mo></x:with-param>
  <x:with-param name="p" select="$p"/>
  <x:with-param name="this-p" select="3"/>
</x:call-template>
</x:template>

<!-- 4.4.5.1 int -->
<x:template match="m:apply[*[1][self::m:int]]
                       |m:apply[*[1][self::m:csymbol='int']]
                       |m:bind[*[1][self::m:int]]
                       |m:bind[*[1][self::m:csymbol='int']]">
 <mrow>
 <msubsup>
  <mi>&#8747;<!--int--></mi>
 <mrow><x:apply-templates select="m:lowlimit/*|m:interval/*[1]|m:condition/*|m:domainofapplication/*"/></mrow>
 <mrow><x:apply-templates select="m:uplimit/*|m:interval/*[2]"/></mrow>
 </msubsup>
 <x:apply-templates select="*[last()]"/>
 <x:if test="m:bvar">
   <mi>d</mi><x:apply-templates select="m:bvar"/>
 </x:if>
</mrow>
</x:template>

<x:template match="m:apply[*[1][self::m:csymbol='defint']]">
<mrow>
<munder><mi>&#8747;<!--int--></mi>
<x:apply-templates select="*[2]"/>
</munder>
 <x:apply-templates select="*[last()]"/>
</mrow>
</x:template>

<!-- 4.4.5.2 diff -->
<x:template match="m:apply[*[1][self::m:diff] and not(m:bvar)]|
                                m:apply[*[1][self::m:csymbol='diff']]" priority="2">
 <msup>
 <mrow><x:apply-templates select="*[2]"/></mrow>
 <mo>&#8242;<!--prime--></mo>
 </msup>
</x:template>

<x:template match="m:apply[*[1][self::m:diff]]" priority="1">
 <mfrac>
 <x:choose>
 <x:when test="m:bvar/m:degree">
 <mrow><msup><mi mathvariant="normal">d</mi><x:apply-templates select="m:bvar/m:degree/node()"/></msup>
     <x:apply-templates  select="*[last()]"/></mrow>
 <mrow><mi mathvariant="normal">d</mi><msup><x:apply-templates
 select="m:bvar/node()"/><x:apply-templates
 select="m:bvar/m:degree/node()"/></msup>
</mrow>
</x:when>
<x:otherwise>
 <mrow><mi mathvariant="normal">d</mi><x:apply-templates select="*[last()]"/></mrow>
 <mrow><mi mathvariant="normal">d</mi><x:apply-templates select="m:bvar"/></mrow>
</x:otherwise>
 </x:choose>
 </mfrac>
</x:template>


<!-- 4.4.5.3 partialdiff -->
<x:template match="m:apply[*[1][self::m:partialdiff] and m:list and not(m:bvar) and count(*)=3]" priority="2">
<mrow>
 <msub><mi>D</mi><mrow>
<x:for-each select="m:list[1]/*">
<x:apply-templates select="."/>
<x:if test="position()&lt;last()"><mo>,</mo></x:if>
</x:for-each>
</mrow></msub>
 <mrow><x:apply-templates select="*[3]"/></mrow>
</mrow>
</x:template>

<x:template match="m:apply[*[1][self::m:partialdiff] and m:list and m:lambda]" priority="3">
  <mfrac>
    <mrow>
      <x:choose>
        <x:when test="count(m:list/*)=1">
          <mo>&#8706;<!-- partial --></mo>
        </x:when>
        <x:otherwise>
          <msup><mo>&#8706;<!-- partial --></mo>
          <mrow>
            <x:choose>
              <x:when test="m:degree">
                <x:apply-templates select="m:degree/node()"/>
              </x:when>
              <x:otherwise>
                <mn><x:value-of select="count(m:list/*)"/></mn>
              </x:otherwise>
            </x:choose>
          </mrow>
          </msup>
        </x:otherwise>
      </x:choose>
    <x:apply-templates  select="m:lambda/*[last()]"/></mrow>
    <mrow>
     <x:call-template name="pddx"/>
    </mrow>
  </mfrac>
</x:template>

<x:template name="pddx">
  <x:param name="n" select="1"/>
  <x:param name="b" select="m:lambda/m:bvar"/>
  <x:param name="l" select="m:list/*"/>
  <x:choose>
    <x:when
       test="number($l[1])=number($l[2])">
     <x:call-template name="pddx">
     <x:with-param name="n" select="$n+1"/>
     <x:with-param name="b" select="$b"/>
     <x:with-param name="l" select="$l[position()!=1]"/>
     </x:call-template>
    </x:when>
   <x:otherwise>
     <mrow>
     <mo>&#8706;<!-- partial --></mo>
     <x:choose>
       <x:when test="$n=1">
         <x:apply-templates select="$b[position()=$l[1]]/*"/>
       </x:when>
       <x:otherwise>
         <msup>
           <x:apply-templates select="$b[position()=$l[1]]/*"/>
           <mn><x:value-of select="$n"/></mn>
         </msup>
       </x:otherwise>
     </x:choose>
     </mrow>
     <x:if test="$l[2]">
      <x:call-template name="pddx">
       <x:with-param name="b" select="$b"/>
       <x:with-param name="l" select="$l[position()!=1]"/>
      </x:call-template>
     </x:if>
   </x:otherwise>
  </x:choose>
</x:template>

<x:template match="m:apply[*[1][self::m:partialdiff]]" priority="1">
  <mfrac>
    <mrow>
      <x:choose>
        <x:when test="not(m:bvar/m:degree) and not(m:bvar[2])">
          <mo>&#8706;<!-- partial --></mo>
        </x:when>
        <x:otherwise>
          <msup><mo>&#8706;<!-- partial --></mo>
          <mrow>
            <x:choose>
              <x:when test="m:degree">
                <x:apply-templates select="m:degree/node()"/>
              </x:when>
              <x:when test="m:bvar/m:degree[string(number(.))='NaN']">
                <x:for-each select="m:bvar/m:degree">
                  <x:apply-templates select="node()"/>
                  <x:if test="position()&lt;last()"><mo>+</mo></x:if>
                </x:for-each>
                <x:if test="count(m:bvar[not(m:degree)])&gt;0">
                  <mo>+</mo><mn><x:value-of select="count(m:bvar[not(m:degree)])"/></mn>
                </x:if>
              </x:when>
              <x:otherwise>
                <mn><x:value-of select="number(sum(m:bvar/m:degree))+count(m:bvar[not(m:degree)])"/></mn>
              </x:otherwise>
            </x:choose>
          </mrow>
          </msup>
        </x:otherwise>
      </x:choose>
    <x:apply-templates  select="*[last()]"/></mrow>
    <mrow>
      <x:for-each select="m:bvar">
        <mrow>
          <mo>&#8706;<!-- partial --></mo><msup><x:apply-templates select="node()"/>
          <mrow><x:apply-templates select="m:degree/node()"/></mrow>
        </msup>
        </mrow>
      </x:for-each>
    </mrow>
  </mfrac>
</x:template>


<x:template match="m:apply[*[1][self::m:csymbol='partialdiffdegree']]">
  <mrow>
   <msub>
    <mo>&#8706;<!-- partial --></mo>
    <mrow>
     <x:apply-templates select="*[2]"/>
    </mrow>
   </msub>
   <mfenced>
     <x:apply-templates select="*[4]"/>
   </mfenced>
  </mrow>
</x:template>


<!-- 4.4.5.4  lowlimit-->
<x:template match="m:lowlimit"/>

<!-- 4.4.5.5 uplimit-->
<x:template match="m:uplimit"/>

<!-- 4.4.5.6  bvar-->
<x:template match="m:bvar">
 <x:apply-templates/>
 <x:if test="following-sibling::m:bvar"><mo>,</mo></x:if>
</x:template>

<!-- 4.4.5.7 degree-->
<x:template match="m:degree"/>

<!-- 4.4.5.8 divergence-->
<x:template match="m:divergence">
<mi>div</mi>
</x:template>

<x:template match="m:apply[*[1][self::m:divergence]and m:bvar and m:vector]">
<x:variable name="v" select="m:bvar"/>
<mrow>
<mi>div</mi>
<mo>&#8289;<!--function application--></mo>
<mo>(</mo>
<mtable>
<x:for-each select="m:vector/*">
<x:variable name="p" select="position()"/>
<mtr><mtd>
<x:apply-templates select="$v[$p]/*"/>
<mo>&#x21a6;<!-- map--></mo>
<x:apply-templates select="."/>
</mtd></mtr>
</x:for-each>
</mtable>
<mo>)</mo>
</mrow>
</x:template>

<!-- 4.4.5.9 grad-->
<x:template match="m:grad">
<mi>grad</mi>
</x:template>

<x:template match="m:apply[*[1][self::m:grad]and m:bvar]">
<mrow>
<mi>grad</mi>
<mo>&#8289;<!--function application--></mo>
<mrow>
<mo>(</mo>
<mfenced>
<x:apply-templates select="m:bvar/*"/>
</mfenced>
<mo>&#x21a6;<!-- map--></mo>
<x:apply-templates select="*[position()!=1][not(self::m:bvar)]"/>
<mo>)</mo>
</mrow>
</mrow>
</x:template>

<!-- 4.4.5.10 curl -->
<x:template match="m:curl">
<mi>curl</mi>
</x:template>


<!-- 4.4.5.11 laplacian-->
<x:template match="m:laplacian">
<msup><mo>&#8711;<!-- nabla --></mo><mn>2</mn></msup>
</x:template>

<x:template match="m:apply[*[1][self::m:laplacian]and m:bvar]">
<mrow>
<x:apply-templates select="*[1]"/>
<mo>&#8289;<!--function application--></mo>
<mrow>
<mo>(</mo>
<mfenced>
<x:apply-templates select="m:bvar/*"/>
</mfenced>
<mo>&#x21a6;<!-- map--></mo>
<x:apply-templates select="*[position()!=1][not(self::m:bvar)]"/>
<mo>)</mo>
</mrow>
</mrow>
</x:template>

<!-- 4.4.6.1 set -->

<x:template match="m:set">
  <x:call-template name="set"/>
</x:template>

<x:template  match="m:apply[*[1][self::m:csymbol='set']]">
<mfenced open="{{" close="}}" separators=",">
  <x:apply-templates select="*[position()!=1]"/>
</mfenced>
</x:template>

<!-- 4.4.6.2 list -->

<x:template match="m:list">
  <x:call-template name="set">
   <x:with-param name="o" select="'('"/>
   <x:with-param name="c" select="')'"/>
  </x:call-template>
</x:template>


<x:template match="m:apply[*[1][self::m:csymbol='list']]">
<mfenced open="(" close=")" separators=",">
  <x:apply-templates select="*[position()!=1]"/>
</mfenced>
</x:template>

<!-- 4.4.6.3 union -->
<x:template match="m:apply[*[1][self::m:union]]
                       |m:apply[*[1][self::m:csymbol='union']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="2"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&#8746;<!-- union --></mo></x:with-param>
</x:call-template>
</x:template>

<x:template match="m:apply[*[1][self::m:union]][m:bvar]
                                |m:apply[*[1][self::m:csymbol='union']][m:bvar]"
              priority="2"
>
  <x:call-template name="sum">
    <x:with-param name="mo"><mo>&#x22C3;</mo></x:with-param>
  </x:call-template>
</x:template>

<!-- 4.4.6.4 intersect -->
<x:template match="m:apply[*[1][self::m:intersect]]
                       |m:apply[*[1][self::m:csymbol='intersect']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="3"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&#8745;<!-- intersect --></mo></x:with-param>
</x:call-template>
</x:template>


<x:template match="m:apply[*[1][self::m:intersect]][m:bvar]
                                |m:apply[*[1][self::m:csymbol='intersect']][m:bvar]"
              priority="2"
>
  <x:call-template name="sum">
    <x:with-param name="mo"><mo>&#x22C2;</mo></x:with-param>
  </x:call-template>
</x:template>



<!-- 4.4.6.5 in -->
<x:template match="m:apply[*[1][self::m:in]]
                       |m:apply[*[1][self::m:csymbol='in']]">
  <x:param name="p" select="0"/>
<x:call-template name="binary">
  <x:with-param name="mo"><mo>&#8712;<!-- in --></mo></x:with-param>
  <x:with-param name="p" select="$p"/>
  <x:with-param name="this-p" select="3"/>
</x:call-template>
</x:template>

<!-- 4.4.6.5 notin -->
<x:template match="m:apply[*[1][self::m:notin]]|m:reln[*[1][self::m:notin]]
                       |m:apply[*[1][self::m:csymbol='notin']]">
  <x:param name="p" select="0"/>
<x:call-template name="binary">
  <x:with-param name="mo"><mo>&#8713;<!-- not in --></mo></x:with-param>
  <x:with-param name="p" select="$p"/>
  <x:with-param name="this-p" select="3"/>
</x:call-template>
</x:template>

<!-- 4.4.6.7 subset -->
<x:template match="m:apply[*[1][self::m:subset]]
                       |m:apply[*[1][self::m:csymbol='subset']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="2"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&#8838;<!-- subseteq --></mo></x:with-param>
</x:call-template>
</x:template>

<!-- 4.4.6.8 prsubset -->
<x:template match="m:apply[*[1][self::m:prsubset]]
                       |m:apply[*[1][self::m:csymbol='prsubset']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="2"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&#8834;<!-- prsubset --></mo></x:with-param>
</x:call-template>
</x:template>

<!-- 4.4.6.9 notsubset -->
<x:template match="m:apply[*[1][self::m:notsubset]]
                       |m:apply[*[1][self::m:csymbol='notsubset']]">
<x:param name="p" select="0"/>
<x:call-template name="binary">
 <x:with-param name="this-p" select="2"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&#8840;<!-- notsubseteq --></mo></x:with-param>
</x:call-template>
</x:template>

<!-- 4.4.6.10 notprsubset -->
<x:template match="m:apply[*[1][self::m:notprsubset]]
                       |m:apply[*[1][self::m:csymbol='notprsubset']]">
<x:param name="p" select="0"/>
<x:call-template name="binary">
 <x:with-param name="this-p" select="2"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&#8836;<!-- prsubset --></mo></x:with-param>
</x:call-template>
</x:template>

<!-- 4.4.6.11 setdiff -->
<x:template match="m:apply[*[1][self::m:setdiff]]
                       |m:apply[*[1][self::m:csymbol='setdiff']]">
<x:param name="p" select="0"/>
<x:call-template name="binary">
 <x:with-param name="this-p" select="2"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&#8726;<!-- setminus --></mo></x:with-param>
</x:call-template>
</x:template>

<!-- 4.4.6.12 card -->
<x:template match="m:apply[*[1][self::m:card]]
                       |m:apply[*[1][self::m:csymbol='card']]">
<mrow>
<mo>|</mo>
<x:apply-templates select="*[2]"/>
<mo>|</mo>
</mrow>
</x:template>

<!-- 4.4.6.13 cartesianproduct -->
<x:template match="m:apply[*[1][self::m:cartesianproduct or self::m:vectorproduct]]
                                |m:apply[*[1][self::m:csymbol[.='cartesian_product' or . = 'vectorproduct']]]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="2"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&#215;<!-- times --></mo></x:with-param>
</x:call-template>
</x:template>

<x:template
match="m:apply[*[1][self::m:cartesianproduct][count(following-sibling::m:reals)=count(following-sibling::*)]]"
priority="2">
<msup>
<x:apply-templates select="*[2]">
  <x:with-param name="p" select="5"/>
</x:apply-templates>
<mn><x:value-of select="count(*)-1"/></mn>
</msup>
</x:template>


<!-- 4.4.7.1 sum -->
<x:template name="sum"  match="m:apply[*[1][self::m:sum]]">
  <x:param name="mo"><mo>&#8721;<!--sum--></mo></x:param>
 <mrow>
 <munderover>
  <x:copy-of select="$mo"/>
 <mrow><x:apply-templates select="m:lowlimit|m:interval/*[1]|m:condition/*|m:domainofapplication/*"/></mrow><!-- Alexey Shamrin shamrinATmail.ru -->
 <mrow><x:apply-templates select="m:uplimit/*|m:interval/*[2]"/></mrow>
 </munderover>
 <x:apply-templates select="*[last()]"/>
</mrow>
</x:template>

<x:template match="m:apply[*[1][self::m:csymbol='sum']]">
<mrow>
<munder><mo>&#8721;<!--sum--></mo>
<x:apply-templates select="*[2]"/>
</munder>
 <x:apply-templates select="*[last()]"/>
</mrow>
</x:template>

<x:template match="m:apply/m:lowlimit" priority="3">
<mrow>
<x:if test="../m:bvar">
  <x:apply-templates select="../m:bvar/node()"/>
  <mo>=</mo>
</x:if>
<x:apply-templates/>
</mrow>
</x:template>

<!-- 4.4.7.2 product -->
<x:template match="m:apply[*[1][self::m:product]]">
  <x:call-template name="sum">
    <x:with-param name="mo"><mo>&#8719;<!--product--></mo></x:with-param>
  </x:call-template>
</x:template>


<x:template match="m:apply[*[1][self::m:csymbol='product']]">
<mrow>
<munder><mo>&#8719;<!--product--></mo>
<x:apply-templates select="*[2]"/>
</munder>
 <x:apply-templates select="*[last()]"/>
</mrow>
</x:template>

<!-- 4.4.7.3 limit -->
<x:template match="m:apply[*[1][self::m:limit]]">
 <mrow>
 <munder>
  <mi>lim</mi> <!-- Alexey Shamrin shamrinATmail.ru -->
 <mrow><x:apply-templates select="m:lowlimit|m:condition/*"/></mrow>
 </munder>
 <x:apply-templates select="*[last()]"/>
</mrow>
</x:template>

<x:template match="m:apply[*[1][self::m:csymbol='limit']][m:bind]">
 <mrow>
 <munder>
  <mi>lim</mi>
 <mrow>
 <x:apply-templates select="m:bind/m:bvar/*"/>
    <mo>
      <x:choose>
        <x:when test="*[3]='above'">&#8600;<!--searrow--></x:when>
        <x:when test="*[3]='below'">&#8599;<!--nearrow--></x:when>
        <x:otherwise>&#8594;<!--rightarrow--></x:otherwise>
      </x:choose>
    </mo>
 <x:apply-templates select="*[2]"/>
</mrow>
 </munder>
 <x:apply-templates select="m:bind/*[last()]"/>
</mrow>
</x:template>



<x:template match="m:apply[m:limit]/m:lowlimit" priority="4">
<mrow>
<x:apply-templates select="../m:bvar/node()"/>
<mo>&#8594;<!--rightarrow--></mo>
<x:apply-templates/>
</mrow>
</x:template>


<!-- 4.4.7.4 tendsto -->
<x:template match="m:apply[*[1][self::m:tendsto]]|m:reln[*[1][self::m:tendsto]]">
 <x:param name="p"/>
<x:call-template name="binary">
 <x:with-param name="this-p" select="2"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>
  <x:choose>
   <x:when test="@type='above'">&#8600;<!--searrow--></x:when>
   <x:when test="@type='below'">&#8599;<!--nearrow--></x:when>
   <x:when test="@type='two-sided'">&#8594;<!--rightarrow--></x:when>
   <x:otherwise>&#8594;<!--rightarrow--></x:otherwise>
  </x:choose>
  </mo></x:with-param>
</x:call-template>
</x:template>

<x:template match="m:apply[*[1][self::m:csymbol='tendsto']]">
  <mrow>
    <x:apply-templates select="*[3]"/>
    <mo>
      <x:choose>
        <x:when test="*[1][self::above]">&#8600;<!--searrow--></x:when>
        <x:when test="*[1][self::below]">&#8599;<!--nearrow--></x:when>
        <x:when test="*[1][self::two-sided]">&#8594;<!--rightarrow--></x:when>
        <x:otherwise>&#8594;<!--rightarrow--></x:otherwise>
      </x:choose>
    </mo>
    <x:apply-templates select="*[4]"/>
  </mrow>
</x:template>

<x:template match="m:apply[*[1][self::m:semantics/m:ci='tendsto']]">
  <mrow>
    <x:apply-templates select="*[2]"/>
    <mo>&#8594;<!--rightarrow--></mo>
    <x:apply-templates select="*[3]"/>
  </mrow>
</x:template>

<x:template match="m:tendsto">
 <mi>tendsto</mi>
</x:template>

<!-- 4.4.8.1 trig -->
<x:template match="m:apply[*[1][
 self::m:sin or self::m:cos or self::m:tan or self::m:sec or
 self::m:csc or self::m:cot or self::m:sinh or self::m:cosh or
 self::m:tanh or self::m:sech or self::m:csch or self::m:coth or
 self::m:arcsin or self::m:arccos or self::m:arctan or self::m:arccosh
 or self::m:arccot or self::m:arccoth or self::m:arccsc or
 self::m:arccsch or self::m:arcsec or self::m:arcsech or
 self::m:arcsinh or self::m:arctanh or self::m:ln]]">
 <x:param name="p" select="0"/>
 <x:param name="op" select="name(../*[1])"/>
  <mrow>
    <x:if test="$p &gt;= 5 and not(m:apply) and $op != 'minus'"><mo>(</mo></x:if>
    <mi><x:value-of select="local-name(*[1])"/></mi>
    <mo>&#8289;<!--function application--></mo>
    <x:if test="m:apply"><mo>(</mo></x:if>
    <x:apply-templates select="*[2]"/>
    <x:if test="m:apply"><mo>)</mo></x:if>
    <x:if test="$p &gt;= 5 and not(m:apply) and $op != 'minus'"><mo>)</mo></x:if>
  </mrow>
</x:template>

<!-- Vasil I. Yaroshevich -->
<x:template match="
 m:sin | m:cos | m:tan | m:sec |
 m:csc | m:cot | m:sinh | m:cosh |
 m:tanh | m:sech | m:csch | m:coth |
 m:arcsin | m:arccos | m:arctan | m:arccosh
 | m:arccot | m:arccoth | m:arccsc |
 m:arccsch | m:arcsec | m:arcsech |
 m:arcsinh | m:arctanh | m:ln|m:mean|
 m:plus|m:minus">
<mi><x:value-of select="local-name()"/></mi>
</x:template>




<!-- 4.4.8.2 exp -->
<x:template match="m:apply[*[1][self::m:exp]]
                       |m:apply[*[1][self::m:csymbol='exp']]">
<x:choose>
 <x:when test="name(../*[1]) = 'power' and name(*[2]) = 'apply'">
  <mrow>
    <mo>(</mo>
    <msup>
      <mi>e<!-- exponential e--></mi>
      <mrow><x:apply-templates select="*[2]"/></mrow>
    </msup>
    <mo>)</mo>
  </mrow>
 </x:when>
 <x:otherwise>
  <msup>
    <mi>e<!-- exponential e--></mi>
    <mrow><x:apply-templates select="*[2]"/></mrow>
  </msup>
 </x:otherwise>
</x:choose>
</x:template>

<!-- 4.4.8.3 ln -->
<!-- with trig -->

<!-- 4.4.8.4 log -->
<x:template match="m:apply[*[1][self::m:log]]
                       |m:apply[*[1][self::m:csymbol='log']]">
  <x:param name="p" select="0"/>
  <x:param name="op" select="name(../*[1])"/>
<mrow>
<x:if test="$p &gt;= 5 and not(m:apply) and $op != 'minus'"><mo>(</mo></x:if>
<x:choose>
<x:when test="not(m:logbase) or number(m:logbase)=10">
<mi>log</mi>
</x:when>
<x:otherwise>
<msub>
<mi>log</mi>
<mrow><x:apply-templates select="m:logbase/node()"/></mrow>
</msub>
</x:otherwise>
</x:choose>
<mo>&#8289;<!--function application--></mo>
<x:if test="m:apply"><mo>(</mo></x:if>
<x:apply-templates select="*[last()]"/>
<x:if test="m:apply"><mo>)</mo></x:if>
<x:if test="$p &gt;= 5 and not(m:apply) and $op != 'minus'"><mo>)</mo></x:if>
</mrow>
</x:template>


<!-- 4.4.9.1 mean -->
<x:template match="m:apply[*[1][self::m:mean]]
                       |m:apply[*[1][self::m:csymbol='mean']]">
<mrow>
 <mo>&#9001;<!--langle--></mo>
    <x:for-each select="*[position()&gt;1]">
      <x:apply-templates select="."/>
      <x:if test="position() !=last()"><mo>,</mo></x:if>
    </x:for-each>
<mo>&#9002;<!--rangle--></mo>
</mrow>
</x:template>


<!-- 4.4.9.2 sdef -->
<x:template match="m:sdev|m:csymbol[.='sdev']">
<mo>&#963;<!--sigma--></mo>
</x:template>

<!-- 4.4.9.3 variance -->
<x:template match="m:apply[*[1][self::m:variance]]
                       |m:apply[*[1][self::m:csymbol='variance']]">
<msup>
<mrow>
<mo>&#963;<!--sigma--></mo>
 <mo>&#8289;<!--function application--></mo>
<mfenced>
<x:apply-templates select="*[position()!=1]"/>
</mfenced>
</mrow>
<mn>2</mn>
</msup>
</x:template>


<!-- 4.4.9.4 median -->
<x:template match="m:median">
<mi>median</mi>
</x:template>


<!-- 4.4.9.5 mode -->
<x:template match="m:mode">
<mi>mode</mi>
</x:template>

<!-- 4.4.9.5 moment -->
<x:template match="m:apply[*[1][self::m:moment]]">
  <mrow>
    <mo>&#9001;<!--langle--></mo>
    <msup>
      <x:variable name="data"
                    select="*[not(position()=1)]
                            [not(self::m:degree or self::m:momentabout)]"/>
      <x:choose>
        <x:when test="$data[2]">
          <mfenced>
            <x:apply-templates select="$data"/>
          </mfenced>
        </x:when>
        <x:otherwise>
          <x:apply-templates select="$data"/>
        </x:otherwise>
      </x:choose>
      <mrow><x:apply-templates select="m:degree/node()"/></mrow>
    </msup>
    <mo>&#9002;<!--rangle--></mo>
  </mrow>
</x:template>

<x:template match="m:apply[*[1][self::m:csymbol='moment']]">
<msub>
  <mrow>
    <mo>&#9001;<!--langle--></mo>
    <msup>
          <x:apply-templates select="*[4]"/>
          <x:apply-templates select="*[2]"/>
    </msup>
    <mo>&#9002;<!--rangle--></mo>
  </mrow>
  <x:apply-templates select="*[3]"/>
</msub>
</x:template>

<!-- 4.4.9.5 momentabout -->
<x:template match="m:momentabout"/>

<x:template match="m:apply[*[1][self::m:moment]][m:momentabout]" priority="2">
  <msub>
    <mrow>
      <mo>&#9001;<!--langle--></mo>
      <msup>
        <x:variable name="data"
                      select="*[not(position()=1)]
                              [not(self::m:degree or self::m:momentabout)]"/>
        <x:choose>
          <x:when test="$data[2]">
            <mfenced>
              <x:apply-templates select="$data"/>
            </mfenced>
          </x:when>
          <x:otherwise>
            <x:apply-templates select="$data"/>
          </x:otherwise>
        </x:choose>
        <mrow><x:apply-templates select="m:degree/node()"/></mrow>
      </msup>
      <mo>&#9002;<!--rangle--></mo>
    </mrow>
    <mrow>
      <x:apply-templates select="m:momentabout/*"/>
    </mrow>
  </msub>
</x:template>

<!-- 4.4.10.1 vector  -->
<x:template match="m:vector">
<mrow>
<mo>(</mo>
<mtable>
<x:for-each select="*">
<mtr><mtd><x:apply-templates select="."/></mtd></mtr>
</x:for-each>
</mtable>
<mo>)</mo>
</mrow>
</x:template>


<x:template match="m:vector[m:condition]">
  <mrow>
    <mo>[</mo>
    <x:apply-templates select="*[last()]"/>
    <mo>|</mo>
    <x:apply-templates select="m:condition"/>
    <mo>]</mo>
  </mrow>
</x:template>

<x:template match="m:vector[m:domainofapplication]">
  <mrow>
    <mo>[</mo>
    <x:apply-templates select="*[last()]"/>
    <mo>|</mo>
    <x:apply-templates select="m:bvar/*"/>
    <mo>&#x2208;</mo>
    <x:apply-templates select="m:domainofapplication/*"/>
    <mo>]</mo>
  </mrow>
</x:template>

<x:template match="m:apply[*[1][self::m:csymbol='vector']]">
<mrow>
<mo>(</mo>
<mtable>
<x:for-each select="*[position()!=1]">
<mtr>
  <mtd><x:apply-templates select="."/></mtd>
</mtr>
</x:for-each>
</mtable>
<mo>)</mo>
</mrow>
</x:template>

<!-- 4.4.10.2 matrix  -->
<x:template match="m:matrix">
<mrow>
<mo>(</mo>
<mtable>
<x:apply-templates/>
</mtable>
<mo>)</mo>
</mrow>
</x:template>

<x:template match="m:matrix[m:condition]">
  <mrow>
    <mo>[</mo>
    <msub>
      <mi>m</mi>
      <mrow>
        <x:for-each select="m:bvar">
          <x:apply-templates/>
          <x:if test="position()!=last()"><mo>,</mo></x:if>
        </x:for-each>
      </mrow>
    </msub>
    <mo>|</mo>
    <mrow>
      <msub>
        <mi>m</mi>
        <mrow>
          <x:for-each select="m:bvar">
            <x:apply-templates/>
            <x:if test="position()!=last()"><mo>,</mo></x:if>
          </x:for-each>
        </mrow>
      </msub>
      <mo>=</mo>
      <x:apply-templates select="*[last()]"/>
    </mrow>
    <mo>;</mo>
    <x:apply-templates select="m:condition"/>
    <mo>]</mo>
  </mrow>
</x:template>

<x:template match="m:apply[*[1][self::m:csymbol='matrix']]">
<mrow>
<mo>(</mo>
<mtable>
<x:apply-templates select="*[position()!=1]"/>
</mtable>
<mo>)</mo>
</mrow>
</x:template>


<!-- 4.4.10.3 matrixrow  -->
<x:template match="m:matrix/m:matrixrow">
<mtr>
<x:for-each select="*">
<mtd><x:apply-templates select="."/></mtd>
</x:for-each>
</mtr>
</x:template>

<x:template match="m:matrixrow">
<mtable>
<mtr>
<x:for-each select="*">
<mtd><x:apply-templates select="."/></mtd>
</x:for-each>
</mtr>
</mtable>
</x:template>

<x:template match="m:apply[*[1][self::m:csymbol.='matrixrow']]">
<mtr>
<x:for-each select="*[position()!=1]">
<mtd><x:apply-templates select="."/></mtd>
</x:for-each>
</mtr>
</x:template>

<!-- 4.4.10.4 determinant  -->
<x:template match="m:apply[*[1][self::m:determinant]]
                       |m:apply[*[1][self::m:csymbol='determinant']]">
<mrow>
<mi>det</mi>
 <mo>&#8289;<!--function application--></mo>
<x:apply-templates select="*[2]">
  <x:with-param name="p" select="7"/>
</x:apply-templates>
</mrow>
</x:template>

<x:template
match="m:apply[*[1][self::m:determinant]][*[2][self::m:matrix]]" priority="2">
<mrow>
<mo>|</mo>
<mtable>
<x:apply-templates select="m:matrix/*"/>
</mtable>
<mo>|</mo>
</mrow>
</x:template>

<!-- 4.4.10.5 transpose -->
<x:template match="m:apply[*[1][self::m:transpose]]
                       |m:apply[*[1][self::m:csymbol='transpose']]">
<msup>
<x:apply-templates select="*[2]">
  <x:with-param name="p" select="7"/>
</x:apply-templates>
<mi>T</mi>
</msup>
</x:template>

<!-- 4.4.10.5 selector -->
<x:template match="m:apply[*[1][self::m:selector]]
                       |m:apply[*[1][self::m:csymbol='selector']]">
<msub>
<x:apply-templates select="*[2]">
  <x:with-param name="p" select="7"/>
</x:apply-templates>
<mrow>
    <x:for-each select="*[position()&gt;2]">
      <x:apply-templates select="."/>
      <x:if test="position() !=last()"><mo>,</mo></x:if>
    </x:for-each>
</mrow>
</msub>
</x:template>

<!-- *** -->
<!-- 4.4.10.6 vectorproduct see cartesianproduct -->


<!-- 4.4.10.7 scalarproduct-->
<x:template match="m:apply[*[1][self::m:scalarproduct]]
                       |m:apply[*[1][self::m:csymbol='scalarproduct']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="2"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>.</mo></x:with-param>
</x:call-template>
</x:template>

<!-- 4.4.10.8 outerproduct-->

<x:template match="m:apply[*[1][self::m:outerproduct]]
                       |m:apply[*[1][self::m:csymbol='outerproduct']]">
<x:param name="p" select="0"/>
<x:call-template name="infix">
 <x:with-param name="this-p" select="2"/>
 <x:with-param name="p" select="$p"/>
 <x:with-param name="mo"><mo>&#x2297;</mo></x:with-param>
</x:call-template>
</x:template>

<!-- 4.4.11.2 semantics -->
<x:template match="m:semantics">
 <x:apply-templates select="*[1]"/>
</x:template>
<x:template match="m:semantics[m:annotation-xml/@encoding='MathML-Presentation']">
 <x:apply-templates select="m:annotation-xml[@encoding='MathML-Presentation']/node()"/>
</x:template>

<!-- 4.4.12.1 integers -->
<x:template match="m:integers">
<mi mathvariant="double-struck">Z</mi>
</x:template>

<!-- 4.4.12.2 reals -->
<x:template match="m:reals">
<mi mathvariant="double-struck">R</mi>
</x:template>

<!-- 4.4.12.3 rationals -->
<x:template match="m:rationals">
<mi mathvariant="double-struck">Q</mi>
</x:template>

<!-- 4.4.12.4 naturalnumbers -->
<x:template match="m:naturalnumbers">
<mi mathvariant="double-struck">N</mi>
</x:template>

<!-- 4.4.12.5 complexes -->
<x:template match="m:complexes">
<mi mathvariant="double-struck">C</mi>
</x:template>

<!-- 4.4.12.6 primes -->
<x:template match="m:primes">
<mi mathvariant="double-struck">P</mi>
</x:template>

<!-- 4.4.12.7 exponentiale -->
<x:template match="m:exponentiale">
  <mi>e<!-- exponential e--></mi>
</x:template>

<!-- 4.4.12.8 imaginaryi -->
<x:template match="m:imaginaryi">
  <mi>i<!-- imaginary i--></mi>
</x:template>

<!-- 4.4.12.9 notanumber -->
<x:template match="m:notanumber">
  <mi>NaN</mi>
</x:template>

<!-- 4.4.12.10 true -->
<x:template match="m:true">
  <mi>true</mi>
</x:template>

<!-- 4.4.12.11 false -->
<x:template match="m:false">
  <mi>false</mi>
</x:template>

<!-- 4.4.12.12 emptyset -->
<x:template match="m:emptyset|m:csymbol[.='emptyset']">
  <mi>&#8709;<!-- emptyset --></mi>
</x:template>


<!-- 4.4.12.13 pi -->
<x:template match="m:pi|m:csymbol[.='pi']">
  <mi>&#960;<!-- pi --></mi>
</x:template>

<!-- 4.4.12.14 eulergamma -->
<x:template match="m:eulergamma|m:csymbol[.='gamma']">
  <mi>&#947;<!-- gamma --></mi>
</x:template>

<!-- 4.4.12.15 infinity -->
<x:template match="m:infinity|m:csymbol[.='infinity']">
  <mi>&#8734;<!-- infinity --></mi>
</x:template>


<!-- ****************************** -->
<x:template name="infix" >
  <x:param name="mo"/>
  <x:param name="p" select="0"/>
  <x:param name="this-p" select="0"/>
  <x:variable name="dmo">
    <x:choose>
     <x:when test="m:domainofapplication">
      <munder>
       <x:copy-of select="$mo"/>
       <mrow>
        <x:apply-templates select="m:domainofapplication/*"/>
       </mrow>
      </munder>
     </x:when>
     <x:otherwise>
       <x:copy-of select="$mo"/>
     </x:otherwise>
    </x:choose>
  </x:variable>
  <mrow>
  <x:if test="$this-p &lt; $p"><mo>(</mo></x:if>
  <x:for-each select="*[not(self::m:domainofapplication)][position()&gt;1]">
   <x:if test="position() &gt; 1">
    <x:copy-of select="$dmo"/>
   </x:if>
   <x:apply-templates select=".">
     <x:with-param name="p" select="$this-p"/>
   </x:apply-templates>
  </x:for-each>
  <x:if test="$this-p &lt; $p"><mo>)</mo></x:if>
  </mrow>
</x:template>

 <!-- make 1-(9-2) google code issue 3-->
<x:template name="binary" >
  <x:param name="mo"/>
  <x:param name="p" select="0"/>
  <x:param name="this-p" select="0"/>
  <x:param name="op" select="name(../*[1])"/>
  <mrow>
  <x:if test="($this-p &lt; $p) or ($this-p=$p and $mo='&#8722;' and $op = 'minus' and (. != ../*[2] or count(../*) = 2) and count(*) != 2)"><mo>(</mo></x:if>
   <x:apply-templates select="*[2]">
     <x:with-param name="p" select="$this-p"/>
   </x:apply-templates>
   <x:copy-of select="$mo"/>
   <x:apply-templates select="*[3]">
     <x:with-param name="p" select="$this-p"/>
   </x:apply-templates>
   <x:if test="($this-p &lt; $p) or ($this-p=$p and $mo='&#8722;' and $op = 'minus' and (. != ../*[2] or count(../*) = 2) and count(*) != 2)"><mo>)</mo></x:if>
  </mrow>
</x:template>

<x:template name="set" >
  <x:param name="o" select="'{'"/>
  <x:param name="c" select="'}'"/>
  <mrow>
   <mo><x:value-of select="$o"/></mo>
   <x:choose>
   <x:when test="m:condition">
   <mrow><x:apply-templates select="m:condition/following-sibling::*"/></mrow>
   <mo>|</mo>
   <mrow><x:apply-templates select="m:condition/node()"/></mrow>
   </x:when>
   <x:when test="m:domainofapplication">
    <mrow><x:apply-templates select="m:domainofapplication/following-sibling::*"/></mrow>
    <mo>|</mo>
    <mrow><x:apply-templates select="m:bvar/node()"/></mrow>
    <mo>&#8712;<!-- in --></mo>
    <mrow><x:apply-templates select="m:domainofapplication/node()"/></mrow>
   </x:when>
   <x:otherwise>
    <x:for-each select="*[not(position()=1 and parent::m:apply)]">
      <x:apply-templates select="."/>
      <x:if test="position() !=last()"><mo>,</mo></x:if>
    </x:for-each>
   </x:otherwise>
   </x:choose>
   <mo><x:value-of select="$c"/></mo>
  </mrow>
</x:template>


<!-- mathml 3 additions -->

<x:template match="m:cs">
  <ms>
   <x:value-of select="
                         translate(.,
                         '&#9;&#10;&#13;&#32;',
                         '&#160;&#160;&#160;&#160;')"/>
 </ms>
</x:template>

<x:template match="m:cbytes">
 <mrow/>
</x:template>

<x:template match="m:cerror">
 <merror>
   <x:apply-templates/>
 </merror>
</x:template>

<x:template match="m:share" priority="4">
 <mi href="{@href}">share<x:value-of select="substring-after(@href,'#')"/></mi>
</x:template>

</x:stylesheet>
