
class Food
{
	
	Atlas * atlas;

	std::vector< Id > ascending_keyframes;
	std::vector< Id >   falling_keyframes;
	
	Point2f  position;
	Vector2f speed;
	
public:

	virtual ~Food() = default;

	const Point2f & get_position () const
	{
		return position;
	}

	const Vector2f & get_speed () const
	{
		return speed;
	}
	
	void set_position (const Point2f & new_position)
	{
		position = new_position;
	}
	
	void update (float time);
	void render (Canvas & canvas);
	
	void apply_impulse (float impulse);
	void contains_point (const Point2f & point);
	
};
