-- test qd library

local qd=require"qd"

------------------------------------------------------------------------------
print(qd.version)

------------------------------------------------------------------------------
function test(w,x,t)
	local s=t
	local S
	for n=1,1000 do
		t=t*x/n
		S=s
		s=s+t
		if s==S then print(w,n,s) return end
	end
	print(w,'*',s)
end

function TEST(x)
	print("\nexp",x)
	test("fp loop",x,1)
	print("fp lib",'',math.exp(x))
	test("qd loop",x,qd.number(1))
	print("qd lib",'',qd.exp(x))
end
TEST(1)
TEST(-1)
TEST(-50)

------------------------------------------------------------------------------
print""
print"Pi algorithm of order 4"

PI=qd.number"3.1415926535897932384626433832795028841971693993751058209749445923078164062862090"

-- http://pauillac.inria.fr/algo/bsolve/constant/pi/pi.html
function A2()
 local x=qd.sqrt(2)
 local p=2+x
 local y=qd.sqrt(x)
 print(-1,p)
 x=(y+1/y)/2
 p=p*(x+1)/(y+1)
 print(0,p)
 for i=1,20 do
  local P=p
  local t=qd.sqrt(x)
  y=(y*t+1/t)/(y+1)
  x=(t+1/t)/2
  p=p*(x+1)/(y+1)
  print(i,p)
  if p==P then break end
 end
 return p
end

p=A2()
print("exact",PI)
print("-",qd.abs(PI-p))

------------------------------------------------------------------------------
print""
print"Square root of 2"

function mysqrt(x)
 local y,z=x,x
 repeat z,y=y,(y+x/y)/2 until z==y
 return y
end

print("f math",math.sqrt(2))
print("f mine",mysqrt(2))
a=qd.sqrt(2) print("B sqrt",a)
b=mysqrt(qd.number(2)) print("B mine",b)
R=qd.number"1.414213562373095048801688724209698078569671875376948073176679737990732478462107038850387534327641573"
print("exact",R)
print(a==b,a<b,a>b,qd.compare(a,b))

------------------------------------------------------------------------------
print""
print"Fibonacci numbers as digits in fraction"

x=99989999
a=qd.div(1,x)
s=qd.tostring(a)
print("1/"..x.." =")
print("",s)
s=string.gsub(s,"0(%d%d%d)"," %1")
print("",s)

------------------------------------------------------------------------------
print""
print"Is exp(pi*sqrt(163)) an integer?"

a=qd.exp(qd.pi*qd.sqrt(163))
print("qd",a,"\nfrac",a-qd.floor(a))
a=math.exp(math.pi*math.sqrt(163))
print("fp",string.format("%.64e",a),"\nfrac",a-math.floor(a))

------------------------------------------------------------------------------
print""
print"Modulo"

qd.digits(10)
A=20.2
B=7.48
a=qd.number(A)
b=qd.number(B)
print("a",a)
print("b",b)
a=qd.number("20.2")
b=qd.number("7.48")
print("a",a)
print("b",b)
print("mod",qd.mod(a,b))
print("MOD",qd.mod(A,B))
print("def",a-qd.floor(a/b)*b)
print("oper",a%b)
print("real",A%B)
print("real",string.format("%.20f",A%B))

------------------------------------------------------------------------------
print""
print"Comparisons"

qd.digits(0)
a=qd.div(4,2)
b=qd.number(1)
print("a","b","a==b","a<b","a>b","qd.compare(a,b)")
print(a,b,a==b,a<b,a>b,qd.compare(a,b))
b=b+1
print(a,b,a==b,a<b,a>b,qd.compare(a,b))
b=b+1
print(a,b,a==b,a<b,a>b,qd.compare(a,b))

------------------------------------------------------------------------------
print""
print(qd.version)

-- eof
