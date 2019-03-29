#ifndef YSSHELLEXT_SNAKEUTIL_IS_INCLUDED
#define YSSHELLEXT_SNAKEUTIL_IS_INCLUDED
/* { */



#include <ysshellext.h>



/*! Snake for feature-edge smoothing.
    Based on:
	  Moonryul Jung and Haengkang Kim
	  "Snaking Across 3D Meshes", 
	  12th Pacific Conference on Computer Graphics and Applications,
	  pp. 87-93,
	  2004
*/
class YsShellExt_SnakeUtil
{
public:
	class EnergyFunction
	{
	public:
		/*! Calculates a vertex energy for vtHd[1].  Vertex vtHd[0] and vtHd[2] are the previous and the next
		    vertex in the snake respectively.  For the first and the last vertex of an open snake, vtHd[0]=nullptr 
		    and vtHd[2]=nullptr respectively.
		*/
		virtual double CalculateEnergy(const YsShellExt &shl,const YsShellExt::VertexHandle vtHd[3]) const=0;
	};
	class Snake
	{
	public:
		/*! Open snake: vtHdChain.front()!=vtHdChain.back().
		    Closed snake: vtHdChain.front()==vtHdChain.back().
		*/
		YsArray <YsShellExt::VertexHandle> vtHdChain;

		void CleanUp(void)
		{
			vtHdChain.CleanUp();
		}
		double CalculateEnergy(const YsShellExt &shl,const EnergyFunction &func) const;
	};

	class DefaultEnergyFunction
	{
	public:
		double alpha,beta;
		DefaultEnergyFunction();
		virtual double CalculateEnergy(const YsShellExt &shl,const YsShellExt::VertexHandle vtHd[3]) const;
	};
	static DefaultEnergyFunction defEnergy;


private:
	Snake snake;
	const YsShellExt::Condition *vtxCond;


public:
	YsShellExt_SnakeUtil();
	~YsShellExt_SnakeUtil();
	void CleanUp(void);



	/*! Start a snake.  Vertices snakeVtHd[0]==snakeVtHd[1] if it is a loop.
	*/
	YSRESULT BeginSnake(const YsShellExt &shl,const YsConstArrayMask <YsShellExt::VertexHandle> snakeVtHd);

	/*! Start a snake.  If snakeVtHd[0] is same as snakeVtHd[nVt-1], it will be taken as a loop regardless of isLoop.
	*/
	YSRESULT BeginSnake(const YsShellExt &shl,YSSIZE_T nVt,const YsShellExt::VertexHandle snakeVtHd[],YSBOOL isLoop);

	/*! Sets a vertex condition.  This class does not own the condition.  Therefore, it is the caller's responsibility to
	    delete the condition after it is no longer used.
	    Give nullptr to clear the vertex condition.
	*/
	YSRESULT SetVertexCondition(const YsShellExt::Condition *vtxCond);
};



/* } */
#endif
