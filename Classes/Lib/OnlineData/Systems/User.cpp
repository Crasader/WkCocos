#include "WkCocos/OnlineData/Systems/User.h"
#include "WkCocos/OnlineData/RequestStatus.h"
#include "WkCocos/OnlineData/Events/Error.h"
#include "WkCocos/OnlineData/Comp/OnlineData.h"

#include "json/document.h"         // rapidjson's DOM-style API
#include "json/stringbuffer.h"
#include "json/writer.h"

namespace WkCocos
{
	namespace OnlineData
	{
		namespace Systems
		{

			User::User()
			{
				m_user_service = ::App42::App42API::BuildUserService();
			}

			User::~User()
			{
				//UserService::Terminate();
				//m_service = nullptr;
			}

			void User::update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt)
			{
				entityx::ptr<Comp::ProgressUpdate> pu;

				entityx::ptr<Comp::Create> c;
				for (auto entity : entities->entities_with_components(c, pu))
				{
					if (!pu->in_progress)
					{
						m_user_service->CreateUser(c->m_userid.c_str(), c->m_passwd.c_str(), c->m_email.c_str(), [entity, events](void* data) mutable
						{//we need only entity here which is just an ID (and can mutate)
							if (entity.valid()) //to be on the safe side
							{
								entityx::ptr<Comp::ProgressUpdate> ecpu = entity.component<Comp::ProgressUpdate>();
								if (ecpu && !ecpu->life_time < TIMEOUT)
								{
									entityx::ptr<Comp::Create> ecc = entity.component<Comp::Create>();
									::App42::App42UserResponse* userdata = static_cast<::App42::App42UserResponse*>(data);
									ImplError httpErr(userdata->httpErrorCode, userdata->errorMessage);
                                    ImplError app42Err(userdata->appErrorCode, userdata->errorDetails);
                                    RequestStatus rs(entity.id(), userdata->isSuccess? Status::REQUEST_SUCCESS : Status::REQUEST_ERROR, { {Impl::HTTP, httpErr}, {Impl::App42,app42Err} } );

									ecc->m_cb(rs);
								}
								CCLOG("Create User entity lived %f seconds", ecpu->life_time);
								entity.destroy();
							}
						});
						pu->in_progress = true;
					}
					else if (pu->life_time > TIMEOUT)
					{
						cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([events, entity](){
							events->emit<Events::Error>(entity.id(), "user create");
						});
						entity.destroy();
					}

				}

				entityx::ptr<Comp::Login> l;
				for (auto entity : entities->entities_with_components(l, pu))
				{
					if (!pu->in_progress)
					{
						m_user_service->Authenticate(l->m_userid.c_str(), l->m_passwd.c_str(), [entity, events](void* data) mutable
						{//we need only entity here which is just an ID (and can mutate)
							if (entity.valid()) //to be on the safe side
							{
								entityx::ptr<Comp::ProgressUpdate> ecpu = entity.component<Comp::ProgressUpdate>();
								if (ecpu && !ecpu->life_time < TIMEOUT)
								{
									entityx::ptr<Comp::Login> ecl = entity.component<Comp::Login>();
									::App42::App42UserResponse* userdata = static_cast<::App42::App42UserResponse*>(data);
                                    ImplError httpErr(userdata->httpErrorCode, userdata->errorMessage);
                                    ImplError app42Err(userdata->appErrorCode, userdata->errorDetails);
                                    RequestStatus rs(entity.id(), userdata->isSuccess? Status::REQUEST_SUCCESS : Status::REQUEST_ERROR, { {Impl::HTTP, httpErr}, {Impl::App42,app42Err} } );

                                    ecl->m_cb(rs,userdata->getBody());
								}
								CCLOG("Login User entity lived %f seconds", ecpu->life_time);
								entity.destroy();
							}
						});
						pu->in_progress = true;
					}
					else if (pu->life_time > TIMEOUT)
					{
                        ImplError wkErr(1, "user login timeout");
                        RequestStatus rs(entity.id(), Status::REQUEST_TIMEOUT, { {Impl::WkCocos ,wkErr} } );
					    l->m_cb(rs,"");
						entity.destroy();
					}

				}

				entityx::ptr<Comp::LoadUserData> lud;
				for (auto entity : entities->entities_with_components(lud, pu))
				{
					if (!pu->in_progress)
					{
						::App42::App42API::setLoggedInUser(lud->m_userid.c_str());
						::App42::App42API::setDbName(DB_NAME);
						m_user_service->setQuery(lud->m_collection.c_str(), NULL); //This will tell
						//App42 that you are requesting all the data linked to the above userName
						m_user_service->GetUser(lud->m_userid.c_str(), [entity, events](void* data) mutable
						{//we need only entity here which is just an ID (and can mutate)
							if (entity.valid()) //to be on the safe side
							{
								entityx::ptr<Comp::ProgressUpdate> ecpu = entity.component<Comp::ProgressUpdate>();
								if (ecpu && !ecpu->life_time < TIMEOUT)
								{
									entityx::ptr<Comp::LoadUserData> eclud = entity.component<Comp::LoadUserData>();
									::App42::App42UserResponse* userdata = static_cast<::App42::App42UserResponse*>(data);
                                    ImplError httpErr(userdata->httpErrorCode, userdata->errorMessage);
                                    ImplError app42Err(userdata->appErrorCode, userdata->errorDetails);
                                    RequestStatus rs(entity.id(), userdata->isSuccess? Status::REQUEST_SUCCESS : Status::REQUEST_ERROR, { {Impl::HTTP, httpErr}, {Impl::App42,app42Err} } );

                                    std::vector<std::string> docs;
                                    std::string docId = "";

                                    if (userdata && ! userdata->users.empty())
                                    {
                                        std::vector<::App42::JSONDocument> jsonDocArray = userdata->users.front().jsonDocArray;
                                        if (!jsonDocArray.empty())
                                        {
                                            //id of last doc is the one we want to use ( others are discarded )
                                            docId = jsonDocArray.back().getDocId();

                                            for (std::vector<::App42::JSONDocument>::iterator it = jsonDocArray.begin(); it != jsonDocArray.end(); ++it)
                                            {
                                                rapidjson::Document doc;
                                                rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
                                                auto jsonDoc = it->getJsonDoc();
                                                docs.push_back(jsonDoc.c_str());
                                            }
                                        }
                                    }
                                    eclud->m_cb(rs, docId, docs);
								}
								CCLOG("Load User Data entity lived %f seconds", ecpu->life_time);
								entity.destroy();
							}
						});
						pu->in_progress = true;
					}
					else if (pu->life_time > TIMEOUT)
					{
						cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([events, entity](){
							events->emit<Events::Error>(entity.id(), "user data load");
						});
						entity.destroy();
					}

				}

			}

		}//namespace Systems
	}//namespace OnlineData
}//namespace WkCocos
