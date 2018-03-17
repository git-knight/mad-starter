#pragma GCC optimize "Ofast,omit-frame-pointer,inline"

#include <iostream>
#include <string>
#include <functional>
#include <algorithm>
#include <math.h>
#include <memory>
#include <chrono>
#include <cstring>
using namespace std;

#define PI 3.14159265358979
#define toDeg 180/PI
#define toRad PI/180

#define sign(x) ((x)<0 ? -1 : ((x>0) ? 1 : 0))
#define clamp(x, a, b)	((x)<a ? a : ((x)>b ? b : (x)))
#define round(x) (int)((x) + 0.5*sign(x))

inline int irand(int a, int b) { return rand() % (b - a + 1) + a; }
inline float frand() { return rand() / (float)RAND_MAX; }
inline float frand(float min, float max) { return frand()*(max - min) + min; }


template<class T>
struct vec2 {
	typedef vec2<T> vec2d;
	T x, y;
	inline vec2<T>() : x(0), y(0) { }
	inline explicit vec2<T>(T v) : x(v), y(v) { }
	inline vec2<T>(T x, T y) : x(x), y(y) { }
	template<class TT> inline vec2<T>(vec2<TT> v) : x(T(v.x)), y(T(v.y)) {}
	inline vec2<T> rounded()const { return vec2<int>(round(x), round(y)); }
	inline T dot(const vec2d v)const { return x*v.x + y*v.y; }
	inline T cross(const vec2d v)const { return y*v.x - x*v.y; }
	inline T lengthSq()const { return dot(*this); }
	inline float length()const { return sqrt(lengthSq()); }
	inline vec2<float> normalized()const { return vec2<float>(x, y) / length(); }
	inline vec2<T> operator+(vec2d b)const { return vec2d(x + b.x, y + b.y); }
	inline vec2<T> operator-(vec2d b)const { return vec2d(x - b.x, y - b.y); }
	template<class T2> inline vec2<T> operator*(T2 n)const { return vec2d(x*n, y*n); }
	template<class T2> inline vec2<T> operator/(T2 n)const { return vec2d(x / n, y / n); }
	inline vec2<T> operator+=(vec2d b) { return *this = vec2d(x + b.x, y + b.y); }
	inline vec2<T> operator-=(vec2d b) { return *this = vec2d(x - b.x, y - b.y); }
	template<class T2> inline vec2<T> operator*=(T2 val) { return *this = vec2d(x * val, y * val); }
	template<class T2> inline vec2<T> operator/=(T2 val) { return *this = vec2d(x / val, y / val); }
	inline bool operator==(vec2d b) { return x == b.x && y == b.y; }
	inline bool operator!=(vec2d b) { return x != b.x || y != b.y; }
};
using vec2d = vec2<float>;
using vec2i = vec2<int>;
inline float distSq(vec2d a, vec2d b) { return (b - a).lengthSq(); }
inline float dist(vec2d a, vec2d b) { return sqrt(distSq(a, b)); }
inline bool isClockwise(vec2d a, vec2d b) { return a.normalized().cross(b.normalized()) <= 0; }
inline float getCosAngle(vec2d a, vec2d b) { return a.normalized().dot(b.normalized()); }
inline float getAngle(vec2d a, vec2d b) { return acos(getCosAngle(a, b))*toDeg; }
inline float getSignedAngle(vec2d a, vec2d b) { a = a.normalized(); b = b.normalized(); return acos(clamp(getCosAngle(a, b), -1, 1))*(isClockwise(a, b) ? -1 : 1); }
inline float getAbsoluteAngle(vec2d vec) { return acos(vec.normalized().dot(vec2d(0, -1)))*sign(vec.x); }



template<class T>
inline std::istream &operator>>(std::istream &is, vec2<T> &m) { return is >> m.x >> m.y; }
template<class T>
inline std::ostream &operator<<(std::ostream &os, vec2<T> m) { return os << m.x << " " << m.y; }

const int maxThrust = 100;
const int depth = 6;

inline float DiscoverCollision(vec2d p1, vec2d v1, vec2d p2, vec2d v2, float rad)
{
	vec2d p21, p;
	float r = rad*rad;

	vec2d v = v1 - v2;
	p21 = p1 - p2;

	if (v.x == 0 && v.y == 0)
		return 10000;

	float a = v.dot(v);
	float b = v.dot(p21) * 2;
	if (b >= 0)
		return 10000;
	float c = p21.dot(p21) - r;

	float d = b*b - 4 * a*c;
	if (d<0)
		return 10000;

	return (-b - sqrt(d)) / (2 * a);
}

struct Checkpoint
{
	int id;
	vec2d loc;
	Checkpoint *prev, *next;

	Checkpoint(){}
	Checkpoint(int id):id(id){cin>>loc;}
};


struct GameMove
{
	float angle, thrust;
	bool boost, shield;

	GameMove(){}
	GameMove(float angle, float thrust, bool shield = false, bool boost = false):angle(angle),thrust(thrust),boost(boost),shield(shield){}
};

struct Pod
{
	vec2d loc, vel;
	Checkpoint* target;
	char lap, shieldTimer, timeout, boostUsed;
	struct { vec2d vec; float angle; } view;

	Pod() :lap(0), shieldTimer(0), timeout(100), boostUsed(0) {}

	void SetAngle(float angle)
	{
		view.angle = angle;
		view.vec = vec2d(cos(angle), sin(angle));
	}

	void ValidateCP()
	{
		if (target->id == 0)
			lap++;

		target = target->next;
		timeout = 100;
	}

	void PrintAction(const GameMove& a)
	{
		int realThrust = Apply(a);
		cout << ((vec2i)(loc + view.vec * 100000)) << " ";
		if (a.shield)
			cout << "SHIELD\n";
		else if (realThrust == 650)
			cout << "BOOST\n";
		else cout << realThrust << endl;
	}

	int Apply(const GameMove& a)
	{
		SetAngle(view.angle + clamp(a.angle, -18 * toRad, 18 * toRad));

		int thrust = (shieldTimer > 0 || a.shield) ? 0 : (a.boost ? (boostUsed ? maxThrust : 650) : clamp((int)a.thrust, 0, maxThrust));
		if (thrust > 0)
			vel += view.vec*thrust;

		if (shieldTimer > 0)
			shieldTimer--;
		if (a.shield)
			shieldTimer = 3;

		boostUsed = boostUsed || a.boost;

		return thrust;
	}
	void MoveBy(float time)
	{
		if (DiscoverCollision(loc, vel, target->loc, vec2d(0), 595) < time)
			ValidateCP();

		loc += vel*time;
	}
	void EndMove()
	{
		vel *= 0.85;
		loc = loc.rounded();
		vel = vec2i(vel);
	}

	GameMove GetMoveAction(vec2d target, float thrust, bool shield = false)
	{
		return GameMove(getSignedAngle(target - loc, view.vec), thrust, shield);
	}
};

void ResolveCollision(Pod* p1, Pod* p2)
{
	float p1Mass = (p1->shieldTimer == 3) ? 10 : 1, p2Mass = (p2->shieldTimer == 3) ? 10 : 1;

	vec2d deltaPos = p2->loc - p1->loc;
	vec2d deltaVel = p2->vel - p1->vel;

	float deltasDot = deltaPos.dot(deltaVel);
	vec2d impact = deltaPos * (deltasDot*p1Mass*p2Mass / (800 * 800 * (p1Mass + p2Mass)));

	p1Mass = 1 / p1Mass;
	p2Mass = 1 / p2Mass;

	p1->vel += impact*p1Mass;
	p2->vel -= impact*p2Mass;

	float impulse = impact.lengthSq();
	if (impulse < 120 * 120)
		impact = impact * (120 / sqrt(impulse));

	p1->vel += impact*p1Mass;
	p2->vel -= impact*p2Mass;
}


void play(Pod* pods)
{
	float timeLeft = 1;
	while (timeLeft > 0)
	{
		float earliest = timeLeft;
		Pod* a = nullptr;
		Pod* b = nullptr;
		for (int i = 0; i < 3; i++)
		{
			for (int j = i + 1; j < 4; j++)
			{
				float t = DiscoverCollision(pods[i].loc, pods[i].vel, pods[j].loc, pods[j].vel, 800);
				if (t < earliest)
				{
					earliest = t;
					a = &pods[i];
					b = &pods[j];
				}
			}
		}

		timeLeft -= earliest;
		for (int i = 0; i < 4; i++)
			pods[i].MoveBy(earliest);

		if (a != nullptr)
			ResolveCollision(a, b);
	}

	for (int i = 0; i < 4; i++)
		pods[i].EndMove();
}










int laps, cpCount;

// function that gives a score to the result of simulating moves
float Evaluate(Pod* pods)
{
	float score = 0;
	for (int p = 0; p < 4; p++)
	{
		int podCpsPassed = pods[p].lap*cpCount + pods[p].target->prev->id;
		int owner = p < 2 ? 1 : -1;
		score += (podCpsPassed * 50000 - dist(pods[p].target->loc, pods[p].loc))*owner; // scoring race completion by all pods, multiplying enemy score by -1
	}

	score -= dist(pods[0].loc, pods[1].loc); // our pods love each other. they want to stay together

	return score;
}

int main()
{
	cin >> laps >> cpCount;

	Checkpoint cps[cpCount];

	for (int i = 0; i < cpCount; i++)
	{
		cps[i].id = i;
		cin >> cps[i].loc;
		cps[i].prev = &cps[(i + cpCount - 1) % cpCount];
		cps[i].next = &cps[(i + 1) % cpCount];
	}

	Pod pods[4];

	for (int i = 0; i < 4; i++)
		pods[i].target = &cps[1];

	while (1)
	{
		for (int i = 0; i < 4; i++)
		{
			int ncp = 0;
			cin >> pods[i].loc >> pods[i].vel >> pods[i].view.angle >> ncp;
			if (ncp == -1)
			{
				cout << "0 0 0\n0 0 0\nEnemy is dead!";
				exit(0);
			}
			while (ncp != pods[i].target->id)
				pods[i].ValidateCP();

			if (pods[i].view.angle != -1)
				pods[i].view.angle *= toRad;
			else pods[i].view.angle = getAbsoluteAngle(cps[1].loc - pods[i].loc);
		}

		GameMove bestMoves[2];
		float bestScore = -10000000000;
		for (int i = 0; i < 50; i++)
		{
			Pod testPods[4];
			memcpy(testPods, pods, sizeof(pods));

			GameMove moves[2];
			for (int m = 0; m < 2; m++)
				moves[m] = GameMove(frand(-36 * toRad, 36 * toRad), frand(-100, maxThrust + 100));

			GameMove FirstTurnMoves[2];
			memcpy(FirstTurnMoves, moves, sizeof(FirstTurnMoves));

			for (int turn = 0; turn < 6; turn++)
			{
				testPods[0].Apply(moves[0]);
				testPods[1].Apply(moves[1]);

				testPods[2].Apply(testPods[2].GetMoveAction(testPods[2].target->loc - testPods[2].vel * 3, 100));
				testPods[3].Apply(testPods[3].GetMoveAction(testPods[3].target->loc - testPods[3].vel * 3, 100));

				play(testPods);

				for (int m = 0; m < 2; m++)
					moves[m] = GameMove(
						moves[m].angle + pow(frand(-3, 3), 3)*toRad, 
						moves[m].thrust + pow(frand(-3, 9), 3)*6
					);
			}

			float score = Evaluate(testPods);

			if (score > bestScore)
			{
				bestScore = score;
				memcpy(bestMoves, FirstTurnMoves, sizeof(FirstTurnMoves));
			}
		}

		for (int p = 0; p < 2; p++)
			pods[p].PrintAction(bestMoves[p]);
	}
}
